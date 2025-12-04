#include "game.h"

Texture2D cardTextures[40];
Texture2D cardBackTexture;

game::game(){
    players[0] = player("Jogador 1");
    players[1] = player("Jogador 2");
    // Initialize PacoteAtual to a safe default (no cards)
    PacoteAtual.isFirst = false;
    PacoteAtual.state = WAIT;
    for (int i = 0; i < 9; ++i) {
        PacoteAtual.cards[i].numero = -1;
        PacoteAtual.cards[i].score = 0;
        PacoteAtual.cards[i].place = hand;
    }
    localPlayPending = false;
    // ensure pendingPacket is initialized as empty
    for (int i = 0; i < 9; ++i) {
        pendingPacket.cards[i].numero = -1;
        pendingPacket.cards[i].score = 0;
        pendingPacket.cards[i].place = hand;
    }
}

void game::StartGame(){
    net.startThread(*this);
}

const vector<carta> &game::getTable() const{
    return mesa;
}

const vector<carta> &game::getTableEnemy() const{
    return mesa_enemy;
}

void game::Draw(){
    interface.Draw(*this);
}

void LoadAllCardTextures() {
    cardBackTexture = LoadTexture("resources/spanish_deck/back.PNG");
    for (int i = 0; i < 40; i++) {
        std::string path = "resources/spanish_deck/" + std::to_string(i+1) + ".PNG";
        Image image = LoadImage(path.c_str());
        cardTextures[i] = LoadTextureFromImage(image);
        UnloadImage(image);
    }
}

void UnloadAllCardTextures() {
    UnloadTexture(cardBackTexture);
    for (int i = 0; i < 40; i++) {
        UnloadTexture(cardTextures[i]);
    }
}

game::~game(){}

void game::Update(){
    // Drain any incoming network packets (thread-safe) and keep the latest
    {
        std::lock_guard<std::mutex> lock(pacoteMutex);
        while (!pacoteQueue.empty()) {
            PacoteAtual = pacoteQueue.front();
            pacoteQueue.pop();
            // Server confirmed our play, clear pending flag
            localPlayPending = false;
        }
    }

    // Apply network packet OR pending packet to keep UI in sync
    ApplyPacoteToHands();
    this->estado = (gamestate)PacoteAtual.state;
    switch ((gamestate)PacoteAtual.state){
        case PLAY:
            SelectHandCard();
            if(IsRectangleClicked(interface.getTrucoButtonRect())){
                SendGamestate(TRUCO);
            }
            break;
        case WAIT:
            break;
        case ROUND_START:
            break;
        case TRUCO:
            if(IsRectangleClicked(interface.getTrucoButtonRect())){
                SendGamestate(RETRUCO);
            }
            if(IsRectangleClicked(interface.getAcceptButtonRect())){
                SendGamestate(TRUCO_ACCEPT);
            }
            if(IsRectangleClicked(interface.getRejectButtonRect())){
                SendGamestate(TRUCO_REJECT);
            }
            break;
        case RETRUCO:
            if(IsRectangleClicked(interface.getAcceptButtonRect())){
                SendGamestate(RETRUCO_ACCEPT);
            }
            if(IsRectangleClicked(interface.getRejectButtonRect())){
                SendGamestate(RETRUCO_REJECT);
            }     
            break;
        case WIN:
            printf("You win!\n");
            break;
        case LOSE:
            printf("You lose!\n");
            break;
        default:
            break;
    }
}

// Map PacoteAtual (or pendingPacket if local play pending) into players' hands and mesa
void game::ApplyPacoteToHands(){
    mesa.clear();
    mesa_enemy.clear();
    players[0].mao.clear();
    players[1].mao.clear();

    // Use pending packet if we have a local play waiting for server confirmation
    PacoteTurno* activePacket = localPlayPending ? &pendingPacket : &PacoteAtual;

    int p0_index = 0;

    for (int i = 0; i < 9; ++i) {
        CardState &cs = activePacket->cards[i];

        if(cs.numero < 0 || cs.place < hand || cs.place > enemy_table || cs.numero > 12) {
            continue;
        }

        card tmp;
        tmp.numero = cs.numero;
        tmp.naipe = cs.naipe;
        tmp.score = cs.score;
        carta ctmp;
        ctmp.cartaToCard(tmp);
        Rectangle table_rect = interface.getRec();

        if(cs.place == table) {
            // My table (sempre table para mim)
            ctmp.setPos(65 + table_rect.x + 100.0f * mesa.size(), table_rect.y + 18.3f);
            mesa.push_back(ctmp);
        } else if (cs.place == hand) {
            // My hand
            ctmp.setPos(p0_index * 100.0f + 180.0f, 650.0f);
            players[0].mao.push_back(ctmp);
            p0_index++;
        } else if (cs.place == enemy_table) {
            // Enemy table (sempre enemy_table para o inimigo)
            ctmp.setPos(65 + table_rect.x + 100.0f * mesa_enemy.size(), table_rect.y - 400 + 18.3f);
            mesa_enemy.push_back(ctmp);
        }
    }
}

void game::PushPacote(const PacoteTurno& p) {
    std::lock_guard<std::mutex> lock(pacoteMutex);
    pacoteQueue.push(p);
}

// Detecta se o usuário clicou em um retângulo qualquer
bool game::IsRectangleClicked(Rectangle rect) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();
        if (CheckCollisionPointRec(mousePos, rect)) {
            return true;
        }
    }
    return false;
}

/// Envia um estado (gamestate) para o servidor
void game::SendGamestate(gamestate newState) {
    PacoteTurno pkt;
    pkt.isFirst = this->PacoteAtual.isFirst;
    pkt.state = newState;
    
    // Initialize all as invalid first
    for (int i = 0; i < 9; ++i) {
        pkt.cards[i].numero = -1;
        pkt.cards[i].score = 0;
        pkt.cards[i].place = hand;
    }
    
    // Preenche o pacote com o contexto atual do cliente
    int idx = 0;
    
    // Adiciona cartas da mão (player 0)
    for (size_t h = 0; h < players[0].mao.size() && idx < 9; ++h) {
        card hc = players[0].mao[h].cardGet();
        pkt.cards[idx].numero = hc.numero;
        pkt.cards[idx].naipe = hc.naipe;
        pkt.cards[idx].score = hc.score;
        pkt.cards[idx].place = hand;
        ++idx;
    }
    
    // Adiciona cartas da mesa (player 0's table)
    for (size_t m = 0; m < mesa.size() && idx < 9; ++m) {
        card mc = mesa[m].cardGet();
        pkt.cards[idx].numero = mc.numero;
        pkt.cards[idx].naipe = mc.naipe;
        pkt.cards[idx].score = mc.score;
        pkt.cards[idx].place = table;
        ++idx;
    }
    
    // CRÍTICO: Salva o pacote e marca como pendente
    this->pendingPacket = pkt;
    this->localPlayPending = true;
    
    this->PacoteAtual.state = WAIT;
    // Envia o pacote ao servidor
    net.sendPlay(*this);
    printf("Sent gamestate %d to server with current context\n", newState);
}

void game::SelectHandCard(){
    static bool arrastando = false;
    static int cartaSelecionada = -1;
    const Rectangle table_rect = interface.getRec();

    if(!arrastando && IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
        Vector2 mp = GetMousePosition();
        printf("MouseDown at %.1f, %.1f\n", mp.x, mp.y);
        for (size_t i = 0; i < players[0].mao.size(); ++i) {
            if (!players[0].mao[i].isActive()) continue;
            if (CheckCollisionPointRec(GetMousePosition(), players[0].mao[i].getRect())) {
                arrastando = true;
                cartaSelecionada = (int)i;
                break;
            }
        }
    }
    
    if(arrastando && IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
        if (cartaSelecionada >= 0 && cartaSelecionada < (int)players[0].mao.size()) {
            Rectangle tem_rec = players[0].mao[cartaSelecionada].getRect();
            players[0].mao[cartaSelecionada].setPos(GetMouseX() - tem_rec.width/2, 
                                                    GetMouseY()-tem_rec.height/2);
        }
    }

    if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
        if(arrastando){
            if(CheckCollisionPointRec(GetMousePosition(),table_rect)){
                if (cartaSelecionada >= 0 && cartaSelecionada < (int)players[0].mao.size()) {
                    // Save the played card
                    card playedCard = players[0].mao[cartaSelecionada].cardGet();

                    // Build pending packet with the NEW state (after removing the card)
                    PacoteTurno pkt;
                    pkt.isFirst = this->PacoteAtual.isFirst;
                    pkt.state = WAIT;
                    
                    // Initialize all as invalid
                    for (int k = 0; k < 9; ++k) {
                        pkt.cards[k].numero = -1;
                        pkt.cards[k].score = 0;
                        pkt.cards[k].place = hand;
                    }

                    // First slot: the played card on table
                    // Cliente sempre joga em "table" (sua própria mesa)
                    pkt.cards[0].numero = playedCard.numero;
                    pkt.cards[0].naipe = playedCard.naipe;
                    pkt.cards[0].score = playedCard.score;
                    pkt.cards[0].place = table;  // Sempre table para o cliente

                    // Remaining slots: cards still in hand (excluding the played one)
                    int k = 1;
                    for (size_t h = 0; h < players[0].mao.size() && k < 9; ++h) {
                        if ((int)h == cartaSelecionada) continue; // skip the played card
                        
                        card hc = players[0].mao[h].cardGet();
                        pkt.cards[k].numero = hc.numero;
                        pkt.cards[k].naipe = hc.naipe;
                        pkt.cards[k].score = hc.score;
                        pkt.cards[k].place = hand;
                        ++k;
                    }

                    // Set pending state - this prevents the old PacoteAtual from resetting the hand
                    this->pendingPacket = pkt;
                    this->localPlayPending = true;
                    
                    // Send to server
                    net.sendPlay(*this);
                    
                }
            } else {
                // Restore positions if dropped outside table
                for (size_t idx = 0; idx < players[0].mao.size(); ++idx) {
                    players[0].mao[idx].setPos(idx*100.0f + 120.0f, 600.0f);
                }
            }
        }
        arrastando = false;
        cartaSelecionada = -1;
    }
}