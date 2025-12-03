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
        PacoteAtual.cards[i].numero = -1; // invalid marker -> ignored by ApplyPacoteToHands
        PacoteAtual.cards[i].score = 0;
        PacoteAtual.cards[i].place = hand;
        // PacoteAtual.cards[i].naipe left uninitialized; numero==-1 will make it ignored
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
    // Start network connection in its own thread to avoid blocking the UI
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

// Função de inicialização das texturas globais
void LoadAllCardTextures() {
    // Carrega a textura da parte de trás da carta
    cardBackTexture = LoadTexture("resources/spanish_deck/back.PNG");

    // Carrega todas as texturas de frente das cartas
    for (int i = 0; i < 40; i++) {
        std::string path = "resources/spanish_deck/" + std::to_string(i+1) + ".PNG";
        Image image = LoadImage(path.c_str());
        cardTextures[i] = LoadTextureFromImage(image);
        UnloadImage(image);
    }
}

// No final do jogo, liberar as texturas globais
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
        }
    }

    // Always try to apply latest network packet to keep UI in sync
    ApplyPacoteToHands();

    switch ((gamestate)PacoteAtual.state){
        case PLAY:
            printf("State: PLAY\n");
            SelectHandCard();
            break;
        case WAIT:
            
            break;
        case ROUND_START:
            
            break;
        default:
            break;
    }
    
}

// Map PacoteAtual into players' hands and the mesa
void game::ApplyPacoteToHands(){
    mesa.clear();
    mesa_enemy.clear();

    // Reseta mao para buildar a partir dos pacotes
    players[0].mao.clear();
    players[1].mao.clear();

    // Index das cartas na mão do jogador 0
    int p0_index = 0;

    Place myplace = enemy_table;
    if(PacoteAtual.isFirst){
        myplace = table;
    }

    for (int i = 0; i < 9; ++i) {
        CardState &cs = PacoteAtual.cards[i];

        if(cs.numero < 0 || cs.place < hand || cs.place > enemy_table || cs.numero > 12) {
            // invalid card slot; skip
            continue;
        }
        printf("Applying card %d to place %d\n", cs.numero, cs.place);

        if(cs.place == myplace) {
            // Card belongs to this player's table
            card tmp;
            tmp.numero = cs.numero;
            tmp.naipe = cs.naipe;
            tmp.score = cs.score;
            carta ctmp;
            ctmp.cartaToCard(tmp);
            Rectangle table_rect = interface.getRec();
            ctmp.setPos(65 + table_rect.x + 100.0f * mesa.size(), table_rect.y + 18.3f);
            mesa.push_back(ctmp);
        } else if (cs.place == hand) {
            // Card belongs to a player's hand
            card tmp;
            tmp.numero = cs.numero;
            tmp.naipe = cs.naipe;
            tmp.score = cs.score;
            carta ctmp;
            ctmp.cartaToCard(tmp);
            // Assign to player 0
            ctmp.setPos(p0_index * 100.0f + 180.0f, 650.0f);
            players[0].mao.push_back(ctmp);
            p0_index++;
        } else{
            // Card belongs to enemy's table
            card tmp;
            tmp.numero = cs.numero;
            tmp.naipe = cs.naipe;
            tmp.score = cs.score;
            carta ctmp;
            ctmp.cartaToCard(tmp);
            Rectangle table_rect = interface.getRec();
            ctmp.setPos(65 + table_rect.x + 100.0f * mesa_enemy.size(), table_rect.y - 400 + 18.3f);
            mesa_enemy.push_back(ctmp);
        }
    }

    printf("Saio \n");

    // If any slots remain (unlikely), ensure they are inactive - not needed with dynamic vector

    // If we have a local pending play, append it visually to the mesa so the local player
    // sees the card immediately while waiting for server confirmation.

    //if (localPlayPending) {
    //    for (int i = 0; i < 9; ++i) {
    //        CardState &pcs = pendingPacket.cards[i];
    //        if (pcs.numero < 0) continue;
    //        // Only append table entries from the pending packet
    //        if (pcs.place == table) {
    //            card tmp;
    //            tmp.numero = pcs.numero;
    //            tmp.naipe = pcs.naipe;
    //            tmp.score = pcs.score;
    //            carta ctmp;
    //            ctmp.cartaToCard(tmp);
    //            Rectangle table_rect = interface.getRec();
    //            ctmp.setPos(65 + table_rect.x + 100.0f * mesa.size(), table_rect.y + 18.3f);
    //            mesa.push_back(ctmp);
    //        }
    //    }
    //}
}

void game::PushPacote(const PacoteTurno& p) {
    std::lock_guard<std::mutex> lock(pacoteMutex);
    pacoteQueue.push(p);
}

// (Queue draining is handled at the start of Update)

// Função para selecionar e arrastar cartas da mão do jogador para a mesa
void game::SelectHandCard(){
    static bool arrastando = false;
    static int cartaSelecionada = -1;
    const Rectangle table_rect = interface.getRec();

    if(!arrastando && IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
        Vector2 mp = GetMousePosition();
        // debug
        printf("MouseDown at %.1f, %.1f\n", mp.x, mp.y);
        for (size_t i = 0; i < players[0].mao.size(); ++i) {
            if (!players[0].mao[i].isActive()) continue; // skip empty slots
            if (CheckCollisionPointRec(GetMousePosition(), players[0].mao[i].getRect())) {
                arrastando = true;
                cartaSelecionada = (int)i;
                printf("Picked card %d\n", (int)i);
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
                // Build a PacoteTurno describing this play before mutating the local slot
                PacoteTurno pkt;
                // mark whether this client believes it's the first player
                pkt.isFirst = this->PacoteAtual.isFirst;
                pkt.state = WAIT; // after playing, wait for server
                // initialize all as invalid
                for (int k = 0; k < 9; ++k) {
                    pkt.cards[k].numero = -1;
                    pkt.cards[k].score = 0;
                    pkt.cards[k].place = hand;
                }
                // copy the played card info
                card played = players[0].mao[cartaSelecionada].cardGet();
                pkt.cards[0].numero = played.numero;
                pkt.cards[0].naipe = played.naipe;
                pkt.cards[0].score = played.score;
                // mark the place according to client's role: if we're first, our table is `table`,
                // otherwise our table is `enemy_table` (server may not tag ownership)
                pkt.cards[0].place = (this->PacoteAtual.isFirst ? table : enemy_table);

                // Remove the card from the local hand so it won't reappear when we build the packet
                if (cartaSelecionada >= 0 && cartaSelecionada < (int)players[0].mao.size()) {
                    // save played card, then erase from hand
                    card playedCard = players[0].mao[cartaSelecionada].cardGet();
                    players[0].mao.erase(players[0].mao.begin() + cartaSelecionada);

                    // Recompute positions for remaining cards in hand
                    for (size_t idx = 0; idx < players[0].mao.size(); ++idx) {
                        players[0].mao[idx].setPos(idx * 100.0f + 120.0f, 600.0f);
                    }

                    // Build a PacoteTurno describing this play based on the new hand (without the played card)
                    // mark whether this client believes it's the first player
                    pkt.isFirst = this->PacoteAtual.isFirst;
                    pkt.state = WAIT; // after playing, wait for server
                    // initialize all as invalid
                    for (int k = 0; k < 9; ++k) {
                        pkt.cards[k].numero = -1;
                        pkt.cards[k].score = 0;
                        pkt.cards[k].place = hand;
                    }
                    // put the played card as first table entry
                    pkt.cards[0].numero = playedCard.numero;
                    pkt.cards[0].naipe = playedCard.naipe;
                    pkt.cards[0].score = playedCard.score;
                    pkt.cards[0].place = (this->PacoteAtual.isFirst ? table : enemy_table);

                    // now serialize remaining hand cards into the packet (starting at index 1)
                    int k = 1;
                    for (size_t h = 0; h < players[0].mao.size() && k < 9; ++h) {
                        card hc = players[0].mao[h].cardGet();
                        pkt.cards[k].numero = hc.numero;
                        pkt.cards[k].naipe = hc.naipe;
                        pkt.cards[k].score = hc.score;
                        pkt.cards[k].place = hand;
                        ++k;
                    }

                    // Set pendingPacket so ApplyPacoteToHands will show our local play until server confirms
                    this->pendingPacket = pkt;
                    this->localPlayPending = true;
                    // send to server (non-blocking behavior handled in network)
                    net.sendPlay(*this);
                }
            }else{
                // Retorna a carta para a posição original se não for colocada na mesa
                // recompute positions to restore
                for (size_t idx = 0; idx < players[0].mao.size(); ++idx) {
                    players[0].mao[idx].setPos(idx*100.0f + 120.0f, 600.0f);
                }
                printf("Dropped card %d outside table\n", cartaSelecionada);
            }
        }
        arrastando = false;
        cartaSelecionada = -1;
    }
}
