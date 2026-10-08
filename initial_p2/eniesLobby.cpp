#include "eniesLobby.h"

// additional var
static Character** globalStrawHats = nullptr;
static int globalStrawHatCount = 0;

/*
 * BattleContext
 */
BattleContext::BattleContext() {
    turnCount = 0;
    morale = 0;
    alarmLevel = 0;
    rescueProgress = 0;
    escapeProgress = 0;
    busterCallTimer = 0;
    mainGateDestroyed = false;
    robinRescued = false;
    bridgeOpened = false;
    battleEnded = false;
    resultCode = "";
}

void BattleContext::nextTurn() {
    this->turnCount++;
}

/*
 * Character
 */
Character::Character() {
    name = ""; hp = 0; maxHp = 0; atk = 0; def = 0; speed = 0; energy = 0;
    alive = false; winThisTurn = false; winBySkill = false; actionThisTurn = false;
}

Character::Character(string name, int hp, int atk, int def, int speed, int energy) {
    this->name = name; this->hp = hp; this->maxHp = hp;
    this->atk = atk; this->def = def; this->speed = speed; this->energy = energy;
    this->alive = (this->hp > 0);
    this->winThisTurn = false; this->winBySkill = false; this->actionThisTurn = false;
}

Character::~Character() {}

int Character::attack(Building* target, BattleContext& context) { return 0; }
int Character::specialSkill(Building* target, BattleContext& context) { return 0; }
void Character::endTurn(BattleContext& context) {}

void Character::receiveDamage(int damage) {
    int realDam = damage - this->def;
    if (realDam < 0) realDam = 0;
    this->hp = clamp(this->hp - realDam, 0, this->maxHp);
    if (this->hp == 0) this->alive = false;    
}

bool Character::isAlive() const { return alive; }
string Character::getName() const { return name; }
int Character::getHP() const { return hp; }
int Character::getMaxHP() const { return maxHp; }
int Character::getAtk() const { return atk; }
int Character::getDef() const { return def; }
int Character::getSpeed() const { return speed; }
int Character::getEnergy() const { return energy; }

void Character::reduceDef(int amount) {
    this->def -= amount;
    if (this->def < 0) this->def = 0; 
}
void Character::reduceSpeed(int amount) {
    this->speed -= amount;
    if (this->speed < 0) this->speed = 0; 
}
void Character::receiveHealing(int amount) {
    this->hp = clamp(this->hp + amount, 0, this->maxHp);
}
bool Character::isStrawHat() const { return false; }
bool Character::isCP9() const { return false; }

/*
 * StrawHat
 */
StrawHat::StrawHat() : Character() { bounty = 0; }
StrawHat::StrawHat(string name, int hp, int atk, int def, int speed, int energy, long long bounty) 
    : Character(name, hp, atk, def, speed, energy), bounty(bounty) {
    static Character* localRegistry[20];
    if (globalStrawHats == nullptr) globalStrawHats = localRegistry;
    if (globalStrawHats == localRegistry) globalStrawHats[globalStrawHatCount++] = this;
}
bool StrawHat::isStrawHat() const { return true; }
string StrawHat::str() const {
    return "StrawHat[name=" + name + ", hp=" + to_string(this->hp) + ", atk=" + to_string(this->atk) + 
           ", def=" + to_string(this->def) + ", speed=" + to_string(this->speed) + 
           ", energy=" + to_string(this->energy) + ", bounty=" + to_string(this->bounty) + "]";
}

/*
 * Luffy
 */
Luffy::Luffy(string name, int hp, int atk, int def, int speed, int energy, long long bounty) 
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}

int Luffy::attack(Character* target, BattleContext& context) {
    if (!target) return 0;
    bool wasAlive = target->isAlive();
    int dam;
    if (this->hp > 0.5 * this->maxHp) dam = this->atk;
    else if (this->hp > 0.3 * this->maxHp && this->hp <= 0.5 * this->maxHp) dam = (int)ceil(1.15 * this->atk);
    else if (this->hp <= 0.3 * this->maxHp) dam = (int)ceil(1.3 * this->atk);
    
    int realDam = dam - target->getDef();
    if (realDam < 0) realDam = 0;
    target->receiveDamage(dam);
    if (wasAlive && !target->isAlive()) {
        context.morale = clamp(context.morale + 5, 0, 100);
        this->winThisTurn = true; 
    }
    return realDam;
}

int Luffy::specialSkill(Character* target, BattleContext& context) {
    if (!target) return 0;
    if (this->energy < 20 || this->hp < 0.15 * this->maxHp) return 0;
    this->energy -= 20;
    bool wasAlive = target->isAlive();
    
    int dam = 2 * this->atk;
    this->atk += 15;
    this->speed += 15;
    context.alarmLevel = clamp(context.alarmLevel + 10, 0, 100);
    int hpLoss = (int)ceil(0.08 * this->maxHp);
    this->hp = clamp(this->hp - hpLoss, 0, this->maxHp);
    if (this->hp == 0) this->alive = false;
    
    int realDam = dam - target->getDef();
    if (realDam < 0) realDam = 0;
    target->receiveDamage(dam);
    if (wasAlive && !target->isAlive()) this->winBySkill = true;
    return realDam;
}

int Luffy::attack(Building* target, BattleContext& context) {
    if (!target) return 0;
    int dam;
    if (this->hp > 0.5 * this->maxHp) dam = this->atk;
    else if (this->hp > 0.3 * this->maxHp && this->hp <= 0.5 * this->maxHp) dam = (int)ceil(1.15 * this->atk);
    else if (this->hp <= 0.3 * this->maxHp) dam = (int)ceil(1.3 * this->atk);
    
    target->receiveDamage(dam);
    if (target->isDestroyed()) target->onDestroyed(context);
    return dam;
}

int Luffy::specialSkill(Building* target, BattleContext& context) {
    if (!target) return 0;
    if (this->energy < 20 || this->hp < 0.15 * this->maxHp) return 0;
    this->energy -= 20;
    
    int dam = 2 * this->atk;
    this->speed += 15;
    this->atk += 15;
    context.alarmLevel = clamp(context.alarmLevel + 10, 0, 100); 
    int hpLoss = (int)ceil(0.08 * this->maxHp);
    this->hp = clamp(this->hp - hpLoss, 0, this->maxHp);
    if (this->hp == 0) this->alive = false;
    
    target->receiveDamage(dam);
    if (target->isDestroyed()) target->onDestroyed(context);
    return dam;
}

void Luffy::endTurn(BattleContext& context) {
    if (this->hp <= 0.3 * this->maxHp) context.morale = clamp(context.morale + 3, 0, 100);
    if (this->winThisTurn || this->winBySkill) {
        this->energy = clamp(this->energy + 5, 0, 100);
    }
    this->winThisTurn = false; this->winBySkill = false; this->actionThisTurn = false;
}

/*
 * Zoro
 */
Zoro::Zoro(string name, int hp, int atk, int def, int speed, int energy, long long bounty) 
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}

int Zoro::attack(Character* target, BattleContext& context) {
    if (!target) return 0;
    bool wasAlive = target->isAlive();
    int dam = this->atk + (int)ceil(0.2 * this->def);
    if (target->getHP() < 0.4 * target->getMaxHP()) dam = (int)ceil(1.15 * dam);
    
    int realDam = dam - target->getDef();
    if (realDam < 0) realDam = 0;
    target->receiveDamage(dam);
    if (wasAlive && !target->isAlive()) this->winThisTurn = true;
    return realDam;
}

int Zoro::specialSkill(Character* target, BattleContext& context) {
    if (!target) return 0;
    if (this->energy < 15) return 0;
    this->energy -= 15;
    bool wasAlive = target->isAlive();
    
    int dam = (int)ceil(2.2 * this->atk);
    if (target->getHP() < 0.5 * target->getMaxHP()) dam = (int)ceil(1.5 * dam);
    
    int realDam = dam - target->getDef();
    if (realDam < 0) realDam = 0;
    target->receiveDamage(dam);
    if (wasAlive && !target->isAlive()) {
        this->energy = clamp(this->energy + 8, 0, 100);
        context.morale = clamp(context.morale + 4, 0, 100); 
        this->winBySkill = true; 
    }
    return realDam;
}

int Zoro::attack(Building* target, BattleContext& context) {
    if (!target) return 0;
    int dam = (int)ceil(this->atk + 0.2 * this->def);
    target->receiveDamage(dam);
    if (target->isDestroyed()) target->onDestroyed(context);
    return dam;
}

int Zoro::specialSkill(Building* target, BattleContext& context) {
    if (!target) return 0;
    if (this->energy < 15) return 0;
    this->energy -= 15;
    int dam = (int)ceil(2.2 * this->atk); 
    target->receiveDamage(dam);
    if (target->isDestroyed()) target->onDestroyed(context);
    return dam;
}

void Zoro::endTurn(BattleContext& context) {
    if (this->winThisTurn || this->winBySkill) {
        context.morale = clamp(context.morale + 6, 0, 100);
        this->atk = (int)ceil(this->atk * 1.05);
    }
    this->winThisTurn = false; this->winBySkill = false; this->actionThisTurn = false;
}

/*
 * Sanji
 */
Sanji::Sanji(string name, int hp, int atk, int def, int speed, int energy, long long bounty) 
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}

int Sanji::attack(Character* target, BattleContext& context) {
    if (!target) return 0;
    bool wasAlive = target->isAlive();
    int dam = this->atk + (int)ceil(0.5 * this->speed);
    if (target->getDef() < this->def) dam = (int)ceil(1.1 * dam);
    
    int realDam = dam - target->getDef();
    if (realDam < 0) realDam = 0;
    target->receiveDamage(dam);
    if (wasAlive && !target->isAlive()) this->winThisTurn = true;
    return realDam;
}

int Sanji::specialSkill(Character* target, BattleContext& context) {
    if (!target) return 0;
    if (this->energy < 18) return 0;
    this->energy -= 18;
    bool wasAlive = target->isAlive();
    int dam = (int)ceil(2.1 * this->atk);
    int realDam = dam - target->getDef();
    if (realDam < 0) realDam = 0;
    target->receiveDamage(dam);
    int amt = (dynamic_cast<Jabra*>(target)) ? 12 : 8;    
    target->reduceDef(amt);
    if (wasAlive && !target->isAlive()) this->winBySkill = true;
    return realDam;
}

int Sanji::attack(Building* target, BattleContext& context) {
    if (!target) return 0;
    int dam = (int)ceil(this->atk + 0.5 * this->speed);
    target->receiveDamage(dam);
    if (target->isDestroyed()) target->onDestroyed(context);
    return dam;
}

int Sanji::specialSkill(Building* target, BattleContext& context) {
    if (!target) return 0;
    if (this->energy < 18) return 0; 
    this->energy -= 18;
    int dam = (int)ceil(2.1 * this->atk); 
    target->receiveDamage(dam);
    if (target->isDestroyed()) target->onDestroyed(context);
    return dam;
}

void Sanji::endTurn(BattleContext& context) {
    if (this->winThisTurn || this->winBySkill) {
        context.morale = clamp(context.morale + 8, 0, 100);
        this->atk = (int)ceil(1.1 * this->atk);
    }
    this->winThisTurn = false; this->winBySkill = false; this->actionThisTurn = false;
}

/*
 * Nami
 */
Nami::Nami(string name, int hp, int atk, int def, int speed, int energy, long long bounty) 
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}

int Nami::attack(Character* target, BattleContext& context) {
    if (!target) return 0;
    bool wasAlive = target->isAlive();
    int effectiveDef = (int)ceil(0.7 * target->getDef());
    int dam = this->atk + (target->getDef() - effectiveDef);
    
    int realDam = dam - target->getDef();
    if (realDam < 0) realDam = 0;
    target->receiveDamage(dam);
    if (wasAlive && !target->isAlive()) this->winThisTurn = true;
    return realDam;
}

int Nami::specialSkill(Character* target, BattleContext& context) {
    if (!target) return 0;
    if (this->energy < 20) return 0;
    this->energy -= 20;
    bool wasAlive = target->isAlive();
    
    int dam = this->atk + 40;
    target->reduceSpeed(10);
    context.busterCallTimer = clamp(context.busterCallTimer + 1, 0, 100);
    context.alarmLevel = clamp(context.alarmLevel - 5, 0, 100);
    
    int realDam = dam - target->getDef();
    if (realDam < 0) realDam = 0;
    target->receiveDamage(dam);
    if (wasAlive && !target->isAlive()) this->winBySkill = true;
    return realDam;
}

int Nami::attack(Building* target, BattleContext& context) {
    if (!target) return 0;
    int dam = (int)ceil(0.5 * this->atk);
    target->receiveDamage(dam);
    if (target->isDestroyed()) target->onDestroyed(context);
    return dam;
}

int Nami::specialSkill(Building* target, BattleContext& context) {
    if (!target) return 0;
    if (this->energy < 20) return 0;
    this->energy -= 20;
    int dam = (int)ceil((this->atk + 40) * 1.5);
    target->receiveDamage(dam);
    context.busterCallTimer = clamp(context.busterCallTimer + 1, 0, 100);
    context.alarmLevel = clamp(context.alarmLevel - 5, 0, 100);
    if (target->isDestroyed()) target->onDestroyed(context);
    return dam;
}

void Nami::endTurn(BattleContext& context) {
    if (this->winThisTurn || this->winBySkill) {
        this->energy = clamp(this->energy + 6, 0, 100);
        context.morale = clamp(context.morale + 5, 0, 100);
    }
    this->winThisTurn = false; this->winBySkill = false; this->actionThisTurn = false;
}

/*
 * Chopper
 */
Chopper::Chopper(string name, int hp, int atk, int def, int speed, int energy, long long bounty) 
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}

int Chopper::attack(Character* target, BattleContext& context) {
    if (!target) return 0;
    bool wasAlive = target->isAlive();
    int realDam = this->atk - target->getDef();
    if (realDam < 0) realDam = 0;
    target->receiveDamage(this->atk);
    if (wasAlive && !target->isAlive()) this->winThisTurn = true;
    return realDam;
}

int Chopper::specialSkill(Character* target, BattleContext& context) {
    if (!target) return 0;
    if (this->energy < 15) return 0;
    this->energy -= 15;
    int heal = 35 + (int)ceil(0.5 * this->atk);
    target->receiveHealing(heal);
    if (dynamic_cast<Luffy*>(target)) context.morale = clamp(context.morale + 5, 0, 100);
    return 0;
}

int Chopper::attack(Building* target, BattleContext& context) {
    if (!target) return 0;
    target->receiveDamage(this->atk);
    if (target->isDestroyed()) target->onDestroyed(context);
    return this->atk;
}

void Chopper::endTurn(BattleContext& context) {
    this->winThisTurn = false; this->winBySkill = false; this->actionThisTurn = false;
}

/*
 * Usopp
 */
Usopp::Usopp(string name, int hp, int atk, int def, int speed, int energy, long long bounty) 
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}

int Usopp::attack(Character* target, BattleContext& context) {
    if (!target) return 0;
    bool wasAlive = target->isAlive();
    int dam = (target->getSpeed() < 50) ? (int)ceil(1.2 * this->atk) : this->atk;
    int realDam = dam - target->getDef();
    if (realDam < 0) realDam = 0;
    
    target->receiveDamage(dam);
    this->actionThisTurn = true;
    if (wasAlive && !target->isAlive()) this->winThisTurn = true;
    return realDam;
}

int Usopp::specialSkill(Character* target, BattleContext& context) {
    if (!target) return 0;
    if (this->energy < 16) return 0;
    this->energy -= 16;
    bool wasAlive = target->isAlive();
    
    int dam = (int)ceil(0.8 * this->atk);
    int realDam = dam - target->getDef();
    if (realDam < 0) realDam = 0;
    
    target->receiveDamage(dam);
    target->reduceSpeed(12);
    context.escapeProgress = clamp(context.escapeProgress + 8, 0, 100);
    this->actionThisTurn = true;
    if (wasAlive && !target->isAlive()) this->winBySkill = true;
    return realDam;
}

int Usopp::attack(Building* target, BattleContext& context) {
    if (!target) return 0;
    int dam = (int)ceil(0.5 * this->atk);
    target->receiveDamage(dam);
    if (target->isDestroyed()) target->onDestroyed(context);
    this->actionThisTurn = true;
    return dam;
}

int Usopp::specialSkill(Building* target, BattleContext& context) {
    if (!target) return 0;
    if (this->energy < 16) return 0;
    this->energy -= 16;
    int dam = (int)ceil(0.8 * this->atk);
    target->receiveDamage(dam);
    if (target->isDestroyed()) target->onDestroyed(context);
    context.escapeProgress = clamp(context.escapeProgress + 8, 0, 100);
    this->actionThisTurn = true;
    return dam;
}

void Usopp::endTurn(BattleContext& context) {
    if (this->actionThisTurn) {
        context.morale = clamp(context.morale + 10, 0, 100);
    }
    this->winThisTurn = false; this->winBySkill = false; this->actionThisTurn = false;
}

/*
 * Franky
 */
Franky::Franky(string name, int hp, int atk, int def, int speed, int energy, long long bounty) 
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}

int Franky::attack(Character* target, BattleContext& context) {
    if (!target) return 0;
    bool wasAlive = target->isAlive();
    int dam = this->atk + (int)ceil(0.3 * this->def);
    if (target->isCP9()) dam = (int)ceil(1.1 * dam);
    
    int realDam = dam - target->getDef();
    if (realDam < 0) realDam = 0;
    target->receiveDamage(dam);
    if (wasAlive && !target->isAlive()) this->winThisTurn = true;
    return realDam;
}

int Franky::specialSkill(Character* target, BattleContext& context) {
    if (!target) return 0;
    bool wasAlive = target->isAlive();
    if (this->energy >= 30) {
        this->energy -= 30;
        int dam = (int)ceil(1.2 * this->atk);
        int realDam = dam - target->getDef();
        if (realDam < 0) realDam = 0;
        target->receiveDamage(dam);
        if (wasAlive && !target->isAlive()) this->winBySkill = true;
        return realDam;
    }
    else if (this->energy >= 20) {
        this->energy -= 20;
        int dam = (int)ceil(1.8 * this->atk);
        target->reduceSpeed(8);
        if (dynamic_cast<Lucci*>(target)) dam = (int)ceil(1.2 * dam);
        int realDam = dam - target->getDef();
        if (realDam < 0) realDam = 0;
        target->receiveDamage(dam);
        if (wasAlive && !target->isAlive()) this->winBySkill = true;
        return realDam;
    }
    return 0;
}

int Franky::attack(Building* target, BattleContext& context) {
    if (!target) return 0;
    int dam = this->atk + (int)ceil(0.3 * this->def);
    target->receiveDamage(dam);
    if (target->isDestroyed()) target->onDestroyed(context);
    return dam;
}

int Franky::specialSkill(Building* target, BattleContext& context) {
    if (!target) return 0;
    if (this->energy >= 30) {
        this->energy -= 30;
        int dam = (int)ceil(1.2 * this->atk);
        target->receiveDamage(target->getHP()); 
        if (target->isDestroyed()) target->onDestroyed(context);
        return dam;
    }
    else if (this->energy >= 20) {
        this->energy -= 20;
        int dam = (int)ceil(1.8 * this->atk);
        target->receiveDamage(dam);
        if (target->isDestroyed()) target->onDestroyed(context);
        return dam;
    }
    return 0;
}

void Franky::endTurn(BattleContext& context) {
    if (this->winThisTurn || this->winBySkill) {
        context.morale = clamp(context.morale + 5, 0, 100);
    }
    if (this->hp > 0.7 * this->maxHp) this->def += 5;
    if (this->hp < 0.3 * this->maxHp) this->atk = (int)ceil(1.1 * this->atk);
    this->winThisTurn = false; this->winBySkill = false; this->actionThisTurn = false;
}

/*
 * CP9Agent
 */
CP9Agent::CP9Agent() : Character() { doriki = 0; }
CP9Agent::CP9Agent(string name, int hp, int atk, int def, int speed, int energy, int doriki) 
    : Character(name, hp, atk, def, speed, energy), doriki(doriki) {}

bool CP9Agent::isCP9() const { return true; }
string CP9Agent::str() const {
    return "CP9[name=" + name + ", hp=" + to_string(this->hp) + ", atk=" + to_string(this->atk) + 
           ", def=" + to_string(this->def) + ", speed=" + to_string(this->speed) + 
           ", energy=" + to_string(this->energy) + ", doriki=" + to_string(this->doriki) + "]";
}

/*
 * Lucci
 */
Lucci::Lucci(string name, int hp, int atk, int def, int speed, int energy, int doriki) 
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}

int Lucci::attack(Character* target, BattleContext& context) {
    if (!target) return 0;
    bool wasAlive = target->isAlive();
    int dam = this->atk + (int)ceil(this->doriki / 20.0);
    if (target->getHP() < 0.5 * target->getMaxHP()) dam = (int)ceil(1.2 * dam);
    
    int realDam = dam - target->getDef();
    if (realDam < 0) realDam = 0;
    target->receiveDamage(dam);
    if (wasAlive && !target->isAlive()) this->winThisTurn = true;
    return realDam;
}

int Lucci::specialSkill(Character* target, BattleContext& context) {
    if (!target) return 0;
    if (this->energy < 25) return 0;
    this->energy -= 25;
    bool wasAlive = target->isAlive();
    
    int baseDam = (int)ceil(2.8 * this->atk);
    int effectiveDef = (int)ceil(0.5 * target->getDef());
    int dam = baseDam + (target->getDef() - effectiveDef);
    
    int realDam = dam - target->getDef();
    if (realDam < 0) realDam = 0;
    target->receiveDamage(dam);
    if (wasAlive && !target->isAlive()) {
        context.morale = clamp(context.morale - 10, 0, 100); // Trừ 10 morale ngay lập tức
        this->winBySkill = true;
    }
    return realDam;
}

void Lucci::endTurn(BattleContext& context) {
    if (this->hp < 0.4 * this->maxHp) this->atk = (int)ceil(1.05 * this->atk);
    this->winThisTurn = false; this->winBySkill = false; this->actionThisTurn = false;
}

/*
 * Kaku
 */
Kaku::Kaku(string name, int hp, int atk, int def, int speed, int energy, int doriki) 
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}

int Kaku::attack(Character* target, BattleContext& context) {
    if (!target) return 0;
    bool wasAlive = target->isAlive();
    int realDam = this->atk - target->getDef();
    if (realDam < 0) realDam = 0;
    target->receiveDamage(this->atk);
    if (wasAlive && !target->isAlive()) this->winThisTurn = true;
    return realDam;
}

int Kaku::specialSkill(Character* target, BattleContext& context) {
    if (!target) return 0;
    if (this->energy < 20) return 0;
    this->energy -= 20;
    
    double rate[3] = {1.2, 1.0, 0.8};
    int totalRealDam = 0;
    bool wasAlive = target->isAlive();
    for (int i = 0; i < 3; i++) {
        if (!target->isAlive()) break;
        int dam = (int)ceil(rate[i] * this->atk);
        int realDam = dam - target->getDef();
        if (realDam < 0) realDam = 0;
        totalRealDam += realDam;
        target->receiveDamage(dam);
        if (wasAlive && !target->isAlive()) {
            this->winBySkill = true;
            wasAlive = false; 
        }
    }
    return totalRealDam;
}

void Kaku::endTurn(BattleContext& context) {
    if (this->winThisTurn || this->winBySkill) {
        context.morale = clamp(context.morale - 5, 0, 100);
    }
    this->winThisTurn = false; this->winBySkill = false; this->actionThisTurn = false;
}

/*
 * Jabra
 */
Jabra::Jabra(string name, int hp, int atk, int def, int speed, int energy, int doriki) 
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}

int Jabra::attack(Character* target, BattleContext& context) {
    if (!target) return 0;
    bool wasAlive = target->isAlive();
    int realDam = this->atk - target->getDef();
    if (realDam < 0) realDam = 0;
    target->receiveDamage(this->atk);
    if (wasAlive && !target->isAlive()) this->winThisTurn = true;
    return realDam;
}

int Jabra::specialSkill(Character* target, BattleContext& context) {
    if (!target) return 0;
    if (this->energy < 18) return 0;
    this->energy -= 18;
    bool wasAlive = target->isAlive();
    
    int dam = (int)ceil(1.5 * this->atk);
    if (this->hp < 0.3 * this->maxHp) dam = (int)ceil(1.25 * dam);
    int realDam = dam - target->getDef();
    if (realDam < 0) realDam = 0;
    target->receiveDamage(dam);
    if (wasAlive && !target->isAlive()) {
        context.morale = clamp(context.morale - 5, 0, 100); // Trừ 5 morale ngay lập tức
        this->winBySkill = true;
    }
    return realDam;
}

void Jabra::endTurn(BattleContext& context) {
    this->winThisTurn = false; this->winBySkill = false; this->actionThisTurn = false;
}

/*
 * Blueno
 */
Blueno::Blueno(string name, int hp, int atk, int def, int speed, int energy, int doriki) 
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}

int Blueno::attack(Character* target, BattleContext& context) {
    if (!target) return 0;
    bool wasAlive = target->isAlive();
    int realDam = this->atk - target->getDef();
    if (realDam < 0) realDam = 0;
    target->receiveDamage(this->atk);
    if (wasAlive && !target->isAlive()) this->winThisTurn = true;
    return realDam;
}

int Blueno::specialSkill(Character* target, BattleContext& context) {
    if (!target) return 0;
    if (this->energy < 15) return 0;
    this->energy -= 15;
    bool wasAlive = target->isAlive();
    
    int dam = (int)ceil(1.3 * this->atk);
    dam += (this->hp > 0.5 * this->maxHp) ? 20 : 40;
    int realDam = dam - target->getDef();
    if (realDam < 0) realDam = 0;
    target->receiveDamage(dam);
    if (wasAlive && !target->isAlive()) this->winBySkill = true;
    return realDam;
}

void Blueno::endTurn(BattleContext& context) {
    if (this->winThisTurn || this->winBySkill) {
        context.morale = clamp(context.morale - 5, 0, 100);
    }
    this->winThisTurn = false; this->winBySkill = false; this->actionThisTurn = false;
}

/*
 * Kalifa
 */
Kalifa::Kalifa(string name, int hp, int atk, int def, int speed, int energy, int doriki) 
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}

int Kalifa::attack(Character* target, BattleContext& context) {
    if (!target) return 0;
    bool wasAlive = target->isAlive();
    int realDam = this->atk - target->getDef();
    if (realDam < 0) realDam = 0;
    target->receiveDamage(this->atk);
    if (wasAlive && !target->isAlive()) this->winThisTurn = true;
    return realDam;
}

int Kalifa::specialSkill(Character* target, BattleContext& context) {
    if (!target) return 0;
    if (this->energy < 18) return 0;
    this->energy -= 18;
    bool wasAlive = target->isAlive();
    int dam = (int)ceil(1.4 * this->atk);
    int realDam = dam - target->getDef();
    if (realDam < 0) realDam = 0;
    target->receiveDamage(dam);
    target->reduceSpeed(6);
    int amt = (dynamic_cast<Nami*>(target)) ? 12 : 8;
    context.morale = clamp(context.morale - amt, 0, 100);
    if (wasAlive && !target->isAlive()) this->winBySkill = true;
    return realDam;
}

void Kalifa::endTurn(BattleContext& context) {
    this->winThisTurn = false; this->winBySkill = false; this->actionThisTurn = false;
}

/*
 * Kumadori
 */
Kumadori::Kumadori(string name, int hp, int atk, int def, int speed, int energy, int doriki) 
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}

int Kumadori::attack(Character* target, BattleContext& context) {
    if (!target) return 0;
    bool wasAlive = target->isAlive();
    int realDam = this->atk - target->getDef();
    if (realDam < 0) realDam = 0;
    target->receiveDamage(this->atk);
    if (wasAlive && !target->isAlive()) this->winThisTurn = true;
    return realDam;
}

int Kumadori::specialSkill(Character* target, BattleContext& context) {
    if (!target) return 0;
    if (this->energy < 16) return 0;
    this->energy -= 16;
    bool wasAlive = target->isAlive();
    
    int dam = 30 + (int)ceil(0.1 * this->doriki);
    if (this->hp < 0.4 * this->maxHp) dam += 25;
    int realDam = dam - target->getDef();
    if (realDam < 0) realDam = 0;
    target->receiveDamage(dam);
    if (wasAlive && !target->isAlive()) this->winBySkill = true;
    return realDam;
}

void Kumadori::endTurn(BattleContext& context) {
    if (this->winThisTurn || this->winBySkill) {
        context.morale = clamp(context.morale - 5, 0, 100);
    }
    this->winThisTurn = false; this->winBySkill = false; this->actionThisTurn = false;
}

/*
 * Fukurou
 */
Fukurou::Fukurou(string name, int hp, int atk, int def, int speed, int energy, int doriki) 
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}

int Fukurou::attack(Character* target, BattleContext& context) {
    if (!target) return 0;
    bool wasAlive = target->isAlive();
    int realDam = this->atk - target->getDef();
    if (realDam < 0) realDam = 0;
    target->receiveDamage(this->atk);
    if (wasAlive && !target->isAlive()) this->winThisTurn = true;
    return realDam;
}

int Fukurou::specialSkill(Character* target, BattleContext& context) {
    if (!target) return 0;
    if (this->energy < 14) return 0;
    this->energy -= 14;
    bool wasAlive = target->isAlive();
    
    int dam = (int)ceil(1.3 * this->atk);
    bool isLowestHP = true;
    if (globalStrawHats != nullptr && globalStrawHatCount > 0) {
        int minHpVal = INT_MAX;
        for (int i = 0; i < globalStrawHatCount; i++) {
            if (globalStrawHats[i] && globalStrawHats[i]->isAlive()) {
                if (globalStrawHats[i]->getHP() < minHpVal) {
                    minHpVal = globalStrawHats[i]->getHP();
                }
            }
        }
        if (target->getHP() > minHpVal) isLowestHP = false; 
    }
    if (isLowestHP) dam += 20;
    
    int realDam = dam - target->getDef();
    if (realDam < 0) realDam = 0;
    target->receiveDamage(dam);
    if (wasAlive && !target->isAlive()) {
        context.morale = clamp(context.morale - 6, 0, 100); // Trừ 6 morale ngay lập tức
        this->winBySkill = true;
    }
    return realDam;
}

void Fukurou::endTurn(BattleContext& context) {
    this->winThisTurn = false; this->winBySkill = false; this->actionThisTurn = false;
}

/*
 * Building
 */
Building::Building(string name, int hp) {
    this->name = name; this->hp = hp; this->maxHP = hp;
    this->destroyed = (hp > 0) ? false : true;
}

Building::~Building() {}

void Building::receiveDamage(int damage) {
    this->hp -= damage;
    if (this->hp <= 0) {
        this->hp = 0;
        this->destroyed = true;
    }
}

bool Building::isDestroyed() const { return destroyed; }
int Building::getHP() const { return hp; }
void Building::onDestroyed(BattleContext& context) { return; }

string Building::str () const {
    return "Building[name=" + this->name + ", hp=" + to_string(this->hp) + 
           ", maxHP=" + to_string(this->maxHP) + ", destroyed=" + (this->destroyed ? "true" : "false") + "]";
}

/*
 * MainGate
 */
MainGate::MainGate(string name, int hp) : Building(name, hp) {}
void MainGate::applyEffect(BattleContext& context) {}
void MainGate::onDestroyed(BattleContext& context) {
    context.mainGateDestroyed = true;
    context.rescueProgress = clamp(context.rescueProgress + 20, 0, 100);
    context.morale = clamp(context.morale + 5, 0, 100);
}

/*
 * Courthouse
 */
Courthouse::Courthouse(string name, int hp) : Building(name, hp) {}
void Courthouse::applyEffect(BattleContext& context) {
    if (!destroyed) context.alarmLevel = clamp(context.alarmLevel + 5, 0, 100);
}
void Courthouse::onDestroyed(BattleContext& context) {
    context.alarmLevel = clamp(context.alarmLevel - 20, 0, 100);
}

/*
 * TowerOfJustice
 */
TowerOfJustice::TowerOfJustice(string name, int hp) : Building(name, hp) {}
void TowerOfJustice::applyEffect(BattleContext& context) {
    if (context.mainGateDestroyed && !context.robinRescued) {
        context.rescueProgress = clamp(context.rescueProgress + 5, 0, 100); 
        if (context.rescueProgress >= 100) {
            context.robinRescued = true;
            context.morale = clamp(context.morale + 10, 0, 100); 
        }
    }
}

/*
 * BridgeOfHesitation
 */
BridgeOfHesitation::BridgeOfHesitation(string name, int hp) : Building(name, hp) {}
void BridgeOfHesitation::applyEffect(BattleContext& context) {
    if (context.robinRescued) {
        context.bridgeOpened = true; 
        context.escapeProgress = clamp(context.escapeProgress + 5, 0, 100); 
        if (context.escapeProgress >= 100) {
            context.battleEnded = true;
            context.resultCode = "STRAW_HAT_WIN";
        }
    }
}

/*
 * BusterCallShip
 */
BusterCallShip::BusterCallShip(string name, int hp) : Building(name, hp) {}
void BusterCallShip::applyEffect(BattleContext& context) {
    if (!destroyed) {
        context.busterCallTimer -= 1;
        if (context.busterCallTimer < 0) context.busterCallTimer = 0; 
        if (context.busterCallTimer == 0) {
            context.battleEnded = true;
            context.resultCode = "BUSTER_CALL";
        }
    }
}
void BusterCallShip::onDestroyed(BattleContext& context) {
    context.busterCallTimer += 3;
}

/*
 * EniesLobbyBattle
 */
EniesLobbyBattle::EniesLobbyBattle(const string& filename) {
    strawHats = new Character*[7]; strawHatCount = 0;
    cp9Agents = new Character*[7]; cp9Count = 0;
    buildings = new Building*[5]; buildingCount = 0;
    turnOrder = nullptr; maxTurns = 0;
    loadFromFile(filename);
}

EniesLobbyBattle::~EniesLobbyBattle() {
    for (int i = 0; i < strawHatCount; i++) delete strawHats[i];
    delete[] strawHats;

    for (int i = 0; i < cp9Count; i++) delete cp9Agents[i];
    delete[] cp9Agents;

    for (int i = 0; i < buildingCount; i++) delete buildings[i];
    delete[] buildings;

    TurnNode* curr = turnOrder;
    while (curr != nullptr) {
        TurnNode* temp = curr;
        curr = curr->next;
        delete temp;
    }
}

void EniesLobbyBattle::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string type;
        ss >> type;
        if (type == "CONTEXT") {
            ss >> context.morale >> context.alarmLevel >> context.rescueProgress 
               >> context.escapeProgress >> context.busterCallTimer >> maxTurns;
        } 
        else if (type == "STRAW_HAT") {
            string name; int hp, atk, def, speed, energy; long long bounty;
            ss >> name >> hp >> atk >> def >> speed >> energy >> bounty;
            Character* member = nullptr;
            if (name == "Luffy") member = new Luffy(name, hp, atk, def, speed, energy, bounty);
            else if (name == "Zoro") member = new Zoro(name, hp, atk, def, speed, energy, bounty);
            else if (name == "Sanji") member = new Sanji(name, hp, atk, def, speed, energy, bounty);
            else if (name == "Nami") member = new Nami(name, hp, atk, def, speed, energy, bounty);
            else if (name == "Chopper") member = new Chopper(name, hp, atk, def, speed, energy, bounty);
            else if (name == "Usopp") member = new Usopp(name, hp, atk, def, speed, energy, bounty);
            else if (name == "Franky") member = new Franky(name, hp, atk, def, speed, energy, bounty);
            if (member) addStrawHat(member);
        } 
        else if (type == "CP9") {
            string name; int hp, atk, def, speed, energy, doriki;
            ss >> name >> hp >> atk >> def >> speed >> energy >> doriki;
            Character* agent = nullptr;
            if (name == "Lucci") agent = new Lucci(name, hp, atk, def, speed, energy, doriki);
            else if (name == "Kaku") agent = new Kaku(name, hp, atk, def, speed, energy, doriki);
            else if (name == "Jabra") agent = new Jabra(name, hp, atk, def, speed, energy, doriki);
            else if (name == "Blueno") agent = new Blueno(name, hp, atk, def, speed, energy, doriki);
            else if (name == "Kalifa") agent = new Kalifa(name, hp, atk, def, speed, energy, doriki);
            else if (name == "Kumadori") agent = new Kumadori(name, hp, atk, def, speed, energy, doriki);
            else if (name == "Fukurou") agent = new Fukurou(name, hp, atk, def, speed, energy, doriki);
            if (agent) addCP9Agent(agent);
        } 
        else if (type == "BUILDING") {
            string name; int hp;
            ss >> name >> hp;
            Building* b = nullptr;
            if (name == "MainGate") b = new MainGate(name, hp);
            else if (name == "Courthouse") b = new Courthouse(name, hp);
            else if (name == "TowerOfJustice") b = new TowerOfJustice(name, hp);
            else if (name == "BridgeOfHesitation") b = new BridgeOfHesitation(name, hp);
            else if (name == "BusterCallShip") b = new BusterCallShip(name, hp);
            if (b) addBuilding(b);
        }
    }
    file.close();
    globalStrawHats = this->strawHats;
    globalStrawHatCount = this->strawHatCount;
    buildTurnOrder();
}

void EniesLobbyBattle::addStrawHat(Character* character) {
    if (strawHatCount < 7) strawHats[strawHatCount++] = character;
}
void EniesLobbyBattle::addCP9Agent(Character* character) {
    if (cp9Count < 7) cp9Agents[cp9Count++] = character;
}
void EniesLobbyBattle::addBuilding(Building* building) {
    if (buildingCount < 5) buildings[buildingCount++] = building;
}

void EniesLobbyBattle::buildTurnOrder() {
    TurnNode* head = nullptr;
    TurnNode* tail = nullptr;
    for (int i = 0; i < strawHatCount; i++) {
        TurnNode* newNode = new TurnNode();
        newNode->data = strawHats[i];
        newNode->next = nullptr;
        if (!head) { head = newNode; tail = newNode; } 
        else { tail->next = newNode; tail = newNode; }
    }
    for (int i = 0; i < cp9Count; i++) {
        TurnNode* newNode = new TurnNode();
        newNode->data = cp9Agents[i];
        newNode->next = nullptr;
        if (!head) { head = newNode; tail = newNode; } 
        else { tail->next = newNode; tail = newNode; }
    }
    turnOrder = head;
}

void EniesLobbyBattle::runBattle() {
    if (!turnOrder) return;
    checkEndCondition();
    while (!context.battleEnded && context.turnCount < maxTurns) {
        TurnNode* currNode = turnOrder;
        Character* currentCharacter = currNode->data;
        if (currentCharacter && currentCharacter->isAlive()) {
            processTurn(currentCharacter);
        }
        if (turnOrder->next != nullptr) {
            turnOrder = turnOrder->next;
            currNode->next = nullptr;
            TurnNode* tail = turnOrder;
            while (tail->next != nullptr) tail = tail->next;
            tail->next = currNode;
        }
        processBuildings(); 
        context.nextTurn();  
        checkEndCondition(); 
    }
}

void EniesLobbyBattle::processTurn(Character* character) {
    if (!character || !character->isAlive()) return;
    Character* targetChar = nullptr;
    Building* targetBuilding = nullptr;
    if (character->isStrawHat()) {
        if (dynamic_cast<Chopper*>(character) && character->getEnergy() >= 15) { 
            Character* lowestSH = nullptr;
            int minHp = INT_MAX;
            for (int i = 0; i < strawHatCount; i++) {
                if (strawHats[i] && strawHats[i]->isAlive() && strawHats[i]->getHP() < minHp) {
                    minHp = strawHats[i]->getHP();
                    lowestSH = strawHats[i];
                }
            }
            targetChar = lowestSH;
        } 
        else {
            Building* mainGate = nullptr;
            Building* courthouse = nullptr;
            Building* busterShip = nullptr;
            Building* bridge = nullptr;
            for (int i = 0; i < buildingCount; i++) {
                if (dynamic_cast<MainGate*>(buildings[i])) mainGate = buildings[i];
                else if (dynamic_cast<Courthouse*>(buildings[i])) courthouse = buildings[i];
                else if (dynamic_cast<BusterCallShip*>(buildings[i])) busterShip = buildings[i];
                else if (dynamic_cast<BridgeOfHesitation*>(buildings[i])) bridge = buildings[i];
            }
            if (mainGate && !mainGate->isDestroyed()) {
                targetBuilding = mainGate;
            }
            else if (courthouse && !courthouse->isDestroyed() && context.alarmLevel >= 50) {
                targetBuilding = courthouse;
            }
            else if (busterShip && !busterShip->isDestroyed() && context.busterCallTimer <= 5) {
                targetBuilding = busterShip;
            }
            else if (!context.robinRescued) {
                for (int i = 0; i < cp9Count; i++) {
                    if (cp9Agents[i] && cp9Agents[i]->isAlive()) {
                        targetChar = cp9Agents[i];
                        break;
                    }
                }
            }
            else {
                if (bridge && !bridge->isDestroyed()) targetBuilding = bridge;
                else {
                    for (int i = 0; i < cp9Count; i++) {
                        if (cp9Agents[i] && cp9Agents[i]->isAlive()) {
                            targetChar = cp9Agents[i];
                            break;
                        }
                    }
                }
            }
        }
    } 
    else if (character->isCP9()) {
        for (int i = 0; i < strawHatCount; i++) {
            if (strawHats[i] && strawHats[i]->isAlive()) {
                targetChar = strawHats[i];
                break;
            }
        }
    }
    bool canCastSkill = false;
    int cost = 0;
    if (dynamic_cast<Luffy*>(character)) cost = 20;
    else if (dynamic_cast<Zoro*>(character)) cost = 15;
    else if (dynamic_cast<Sanji*>(character)) cost = 18;
    else if (dynamic_cast<Nami*>(character)) cost = 20;
    else if (dynamic_cast<Chopper*>(character)) cost = 15;
    else if (dynamic_cast<Usopp*>(character)) cost = 16;
    else if (dynamic_cast<Lucci*>(character)) cost = 25;
    else if (dynamic_cast<Kaku*>(character)) cost = 20;
    else if (dynamic_cast<Jabra*>(character)) cost = 18;
    else if (dynamic_cast<Blueno*>(character)) cost = 15;
    else if (dynamic_cast<Kalifa*>(character)) cost = 18;
    else if (dynamic_cast<Kumadori*>(character)) cost = 16;
    else if (dynamic_cast<Fukurou*>(character)) cost = 14;
    if (dynamic_cast<Franky*>(character)) {
        if (character->getEnergy() >= 20) canCastSkill = true; 
    } else if (character->getEnergy() >= cost) {
        canCastSkill = true;
    }
    if (canCastSkill) {
        if (targetChar) character->specialSkill(targetChar, context); 
        else if (targetBuilding) character->specialSkill(targetBuilding, context);
    } else {
        if (targetChar) character->attack(targetChar, context); 
        else if (targetBuilding) character->attack(targetBuilding, context);
    }
    character->endTurn(context); 
}

void EniesLobbyBattle::processBuildings() {
    for (int i = 0; i < buildingCount; i++) {
        if (buildings[i] && !buildings[i]->isDestroyed()) {
            buildings[i]->applyEffect(context);
        }
    }
}

void EniesLobbyBattle::checkEndCondition() {
    if (context.robinRescued && context.escapeProgress >= 100) {
        context.battleEnded = true;
        context.resultCode = "STRAW_HAT_WIN";
        return;
    }
    if (context.busterCallTimer <= 0) {
        context.battleEnded = true;
        context.resultCode = "BUSTER_CALL";
        return;
    }
    bool shAlive = false;
    for (int i = 0; i < strawHatCount; i++) {
        if (strawHats[i] && strawHats[i]->isAlive()) { shAlive = true; break; }
    }
    if (!shAlive) {
        context.battleEnded = true;
        context.resultCode = "CP9_WIN";
        return;
    }
    bool cp9Alive = false;
    for (int i = 0; i < cp9Count; i++) {
        if (cp9Agents[i] && cp9Agents[i]->isAlive()) { cp9Alive = true; break; }
    }
    if (!cp9Alive) {
        context.battleEnded = true;
        context.resultCode = "STRAW_HAT_WIN_BY_DEFEAT_CP9";
        return;
    }
    if (context.turnCount >= maxTurns) {
        context.battleEnded = true;
        context.resultCode = "TIME_OUT";
        return;
    }
}

string EniesLobbyBattle::getResult() const {
    return context.resultCode + " " + 
           to_string(context.turnCount) + " " +
           to_string(context.morale) + " " + 
           to_string(context.alarmLevel) + " " +
           to_string(context.rescueProgress) + " " + 
           to_string(context.escapeProgress) + " " +
           to_string(context.busterCallTimer);
}