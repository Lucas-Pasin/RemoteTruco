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
    // Basic assumptions (documented):
    // - PacoteAtual.cards contains CardState entries where CardState.place indicates hand or table.
    // - We will assign the first up-to-3 hand cards to players[0].mao from left to right,
    //   then the next up-to-3 hand cards to players[1].mao. Cards with invalid numero (<0)
    //   will result in an inactive slot.

    // Clear current table and rebuild from packet
    mesa.clear();
    mesa_enemy.clear();

    int p0_index = 0;
    int p1_index = 0;

    // Reset all hand slots to inactive before applying
    for (int i = 0; i < 3; ++i) {
        players[0].mao[i].setActive(false);
        players[1].mao[i].setActive(false);
    }

    for (int i = 0; i < 9; ++i) {
        CardState &cs = PacoteAtual.cards[i];

        // ignore placeholder/invalid cards
        if (cs.numero < 0) continue;

        if (cs.place == table) {
            // Interpret table ownership: if the packet indicates isFirst, then table entries
            // belong to the player who is 'first'. If PacoteAtual.isFirst differs from our
            // local role, we should render those as enemy_table for the local UI.
            bool serverMarksFirst = PacoteAtual.isFirst;
            // If server says this packet's 'first' is not us, then entries with 'table'
            // should be rendered as enemy_table for us. We'll render accordingly by
            // converting to enemy_table when ownership doesn't match.
            bool convertToEnemy = false;
            // Determine local role: assume our client role == PacoteAtual.isFirst (best-effort)
            if (serverMarksFirst != this->PacoteAtual.isFirst) convertToEnemy = true;
            // NOTE: Above line is conservative; ideally the client should know its own role
            // from connection/login flow. Here we keep behavior simple.
            // Convert to carta and push to mesa
            card temp;
            temp.numero = cs.numero;
            temp.naipe = cs.naipe;
            temp.score = cs.score;

            // create a carta and set its texture
            carta c;
            c.cartaToCard(temp);
            // Position the card on the table
            Rectangle table_rect = interface.getRec();
            float x = 65 + table_rect.x + 100.0f * mesa.size();
            float y = table_rect.y + 18.3f;
            c.setPos(x, y);
            mesa.push_back(c);
        } else if (cs.place == enemy_table) {
            // enemy table card: position separately (e.g., opposite side of table)
            card temp_e;
            temp_e.numero = cs.numero;
            temp_e.naipe = cs.naipe;
            temp_e.score = cs.score;

            carta c_e;
            c_e.cartaToCard(temp_e);
            // Place enemy cards near the top between players
            Rectangle table_rect = interface.getRec();
            float x_e = 65 + table_rect.x + 100.0f * mesa.size();
            float y_e = table_rect.y + 18.3f - 150.0f; // offset upward for enemy
            c_e.setPos(x_e, y_e);
            mesa.push_back(c_e);
        } else if (cs.place == hand) {
            card temp;
            temp.numero = cs.numero;
            temp.naipe = cs.naipe;
            temp.score = cs.score;

            if (p0_index < 3) {
                // assign to player 0
                players[0].mao[p0_index].cartaToCard(temp);
                players[0].mao[p0_index].setPos(p0_index*100.0f + 120.0f, 600.0f);
                players[0].mao[p0_index].setActive(true);
                ++p0_index;
            } else if (p1_index < 3) {
                // assign to player 1
                players[1].mao[p1_index].cartaToCard(temp);
                players[1].mao[p1_index].setPos(p1_index*100.0f + 120.0f, 100.0f);
                players[1].mao[p1_index].setActive(true);
                ++p1_index;
            } else {
                // extra hand cards ignored
            }
        }
    }

    // Deactivate any remaining slots that didn't receive a card
    for (int i = p0_index; i < 3; ++i) players[0].mao[i].setActive(false);
    for (int i = p1_index; i < 3; ++i) players[1].mao[i].setActive(false);

    // If we have a local pending play, append it visually to the mesa so the local player
    // sees the card immediately while waiting for server confirmation.
    if (localPlayPending) {
        for (int i = 0; i < 9; ++i) {
            CardState &pcs = pendingPacket.cards[i];
            if (pcs.numero < 0) continue;
            // Only append table entries from the pending packet
            if (pcs.place == table) {
                card tmp;
                tmp.numero = pcs.numero;
                tmp.naipe = pcs.naipe;
                tmp.score = pcs.score;
                carta ctmp;
                ctmp.cartaToCard(tmp);
                Rectangle table_rect = interface.getRec();
                ctmp.setPos(65 + table_rect.x + 100.0f * mesa.size(), table_rect.y + 18.3f);
                mesa.push_back(ctmp);
            }
        }
    }
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
        for(int i=0;i<3;i++){
            if (!players[0].mao[i].isActive()) continue; // skip empty slots
            if(CheckCollisionPointRec(GetMousePosition(), players[0].mao[i].getRect())){
                arrastando = true;
                cartaSelecionada = i;
                printf("Picked card %d\n", i);
                break;
            }
        }
    }
    
    if(arrastando && IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
        if (cartaSelecionada >= 0 && cartaSelecionada < 3) {
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

                // Update local UI: place on table and mark slot empty
                players[0].mao[cartaSelecionada].setPos(900.0f,900.0f);
                players[0].mao[cartaSelecionada].setActive(false);
                // position the visible card on mesa
                played.numero = pkt.cards[0].numero; // ensure consistency
                carta shown;
                shown.cartaToCard(played);
                shown.setPos(65 + table_rect.x + 100.0f * mesa.size(), table_rect.y + 18.3f);
                mesa.push_back(shown);

                // Set pendingPacket so ApplyPacoteToHands will show our local play until server confirms
                this->pendingPacket = pkt;
                this->localPlayPending = true;
                // send to server (non-blocking behavior handled in network)
                net.sendPlay(*this);
            }else{
                // Retorna a carta para a posição original se não for colocada na mesa
                players[0].mao[cartaSelecionada].setPos(cartaSelecionada*100.0f + 260.0f, 680.0f);
                printf("Dropped card %d outside table\n", cartaSelecionada);
            }
        }
        arrastando = false;
        cartaSelecionada = -1;
    }
}
