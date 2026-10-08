#ifndef ENIES_LOBBY_H
#define ENIES_LOBBY_H

#include "main.h"

class BattleContext;
class Building;
class Character;
struct TurnNode;

// Additional function
inline int clamp(int val, int low, int high) {
    if (val < low) return low;
    if (val > high) return high;
    return val;
}

/*
 * Character
 */
class Character {
protected:
    string name;
    int hp;
    int maxHp;
    int atk;
    int def;
    int speed;
    int energy;
    bool alive;
    
    // flag for endturn
    bool winThisTurn;
    bool winBySkill;
    bool actionThisTurn;

public:
    Character();
    Character(string name, int hp, int atk, int def, int speed, int energy);
    virtual ~Character();

    virtual int attack(Character* target, BattleContext& context) = 0;
    virtual int specialSkill(Character* target, BattleContext& context) = 0;

    virtual int attack(Building* target, BattleContext& context);
    virtual int specialSkill(Building* target, BattleContext& context);

    virtual void endTurn(BattleContext& context);

    void receiveDamage(int damage);
    bool isAlive() const;
    string getName() const;
    int getHP() const;
    int getMaxHP() const;
    int getAtk() const;
    int getDef() const;
    int getSpeed() const;
    int getEnergy() const;

    // addition methods
    void reduceDef(int amount);
    void reduceSpeed(int amount);
    void receiveHealing(int amount);

    virtual bool isStrawHat() const;
    virtual bool isCP9() const;

    virtual string str() const = 0;
};

/*
 * StrawHat
 */
class StrawHat : public Character {
protected:
    long long bounty;

public:
    StrawHat();
    StrawHat(string name, int hp, int atk, int def,
             int speed, int energy, long long bounty);

    virtual bool isStrawHat() const;
    virtual string str() const;
};

/*
 * Straw Hat members
 */
class Luffy : public StrawHat {
public:
    Luffy(string name, int hp, int atk, int def, int speed, int energy, long long bounty);
    int attack(Character* target, BattleContext& context);
    int specialSkill(Character* target, BattleContext& context);
    int attack(Building* target, BattleContext& context);
    int specialSkill(Building* target, BattleContext& context);
    void endTurn(BattleContext& context);
};

class Zoro : public StrawHat {
public:
    Zoro(string name, int hp, int atk, int def, int speed, int energy, long long bounty);
    int attack(Character* target, BattleContext& context);
    int specialSkill(Character* target, BattleContext& context);
    int attack(Building* target, BattleContext& context);
    int specialSkill(Building* target, BattleContext& context);
    void endTurn(BattleContext& context);
};

class Sanji : public StrawHat {
public:
    Sanji(string name, int hp, int atk, int def, int speed, int energy, long long bounty);
    int attack(Character* target, BattleContext& context);
    int specialSkill(Character* target, BattleContext& context);
    int attack(Building* target, BattleContext& context);
    int specialSkill(Building* target, BattleContext& context);
    void endTurn(BattleContext& context);
};

class Nami : public StrawHat {
public:
    Nami(string name, int hp, int atk, int def, int speed, int energy, long long bounty);
    int attack(Character* target, BattleContext& context);
    int specialSkill(Character* target, BattleContext& context);
    int attack(Building* target, BattleContext& context);
    int specialSkill(Building* target, BattleContext& context);
    void endTurn(BattleContext& context);
};

class Chopper : public StrawHat {
public:
    Chopper(string name, int hp, int atk, int def, int speed, int energy, long long bounty);
    int attack(Character* target, BattleContext& context);
    int specialSkill(Character* target, BattleContext& context);
    int attack(Building* target, BattleContext& context);
    void endTurn(BattleContext& context);
};

class Usopp : public StrawHat {
public:
    Usopp(string name, int hp, int atk, int def, int speed, int energy, long long bounty);
    int attack(Character* target, BattleContext& context);
    int specialSkill(Character* target, BattleContext& context);
    int attack(Building* target, BattleContext& context);
    int specialSkill(Building* target, BattleContext& context);
    void endTurn(BattleContext& context);
};

class Franky : public StrawHat {
public:
    Franky(string name, int hp, int atk, int def, int speed, int energy, long long bounty);
    int attack(Character* target, BattleContext& context);
    int specialSkill(Character* target, BattleContext& context);
    int attack(Building* target, BattleContext& context);
    int specialSkill(Building* target, BattleContext& context);
    void endTurn(BattleContext& context);
};

/*
 * CP9Agent
 */
class CP9Agent : public Character {
protected:
    int doriki;

public:
    CP9Agent();
    CP9Agent(string name, int hp, int atk, int def, int speed, int energy, int doriki);
    virtual bool isCP9() const;
    virtual string str() const;
};

/*
 * CP9 members
 */
class Lucci : public CP9Agent {
public:
    Lucci(string name, int hp, int atk, int def, int speed, int energy, int doriki);
    int attack(Character* target, BattleContext& context);
    int specialSkill(Character* target, BattleContext& context);
    void endTurn(BattleContext& context);
};

class Kaku : public CP9Agent {
public:
    Kaku(string name, int hp, int atk, int def, int speed, int energy, int doriki);
    int attack(Character* target, BattleContext& context);
    int specialSkill(Character* target, BattleContext& context);
    void endTurn(BattleContext& context);
};

class Jabra : public CP9Agent {
public:
    Jabra(string name, int hp, int atk, int def, int speed, int energy, int doriki);
    int attack(Character* target, BattleContext& context);
    int specialSkill(Character* target, BattleContext& context);
    void endTurn(BattleContext& context);
};

class Blueno : public CP9Agent {
public:
    Blueno(string name, int hp, int atk, int def, int speed, int energy, int doriki);
    int attack(Character* target, BattleContext& context);
    int specialSkill(Character* target, BattleContext& context);
    void endTurn(BattleContext& context);
};

class Kalifa : public CP9Agent {
public:
    Kalifa(string name, int hp, int atk, int def, int speed, int energy, int doriki);
    int attack(Character* target, BattleContext& context);
    int specialSkill(Character* target, BattleContext& context);
    void endTurn(BattleContext& context);
};

class Kumadori : public CP9Agent {
public:
    Kumadori(string name, int hp, int atk, int def, int speed, int energy, int doriki);
    int attack(Character* target, BattleContext& context);
    int specialSkill(Character* target, BattleContext& context);
    void endTurn(BattleContext& context);
};

class Fukurou : public CP9Agent {
public:
    Fukurou(string name, int hp, int atk, int def, int speed, int energy, int doriki);
    int attack(Character* target, BattleContext& context);
    int specialSkill(Character* target, BattleContext& context);
    void endTurn(BattleContext& context);
};

/*
 * BattleContext
 */
class BattleContext {
public:
    int turnCount;
    int morale;
    int alarmLevel;
    int rescueProgress;
    int escapeProgress;
    int busterCallTimer;
    bool mainGateDestroyed;
    bool robinRescued;
    bool bridgeOpened;
    bool battleEnded;
    string resultCode;

public:
    BattleContext();
    void nextTurn();
};

/*
 * Building
 */
class Building {
protected:
    string name;
    int hp;
    int maxHP;
    bool destroyed;

public:
    Building(string name, int hp);
    virtual ~Building();
    void receiveDamage(int damage);
    bool isDestroyed() const;
    int getHP() const;
    virtual void applyEffect(BattleContext& context) = 0;
    virtual void onDestroyed(BattleContext& context);
    virtual string str () const;
};

/*
 * Building subclasses
 */
class MainGate : public Building {
public:
    MainGate(string name, int hp);
    void applyEffect(BattleContext& context);
    void onDestroyed(BattleContext& context);
};

class Courthouse : public Building {
public:
    Courthouse(string name, int hp);
    void applyEffect(BattleContext& context);
    void onDestroyed(BattleContext& context);
};

class TowerOfJustice : public Building {
public:
    TowerOfJustice(string name, int hp);
    void applyEffect(BattleContext& context);
};

class BridgeOfHesitation : public Building {
public:
    BridgeOfHesitation(string name, int hp);
    void applyEffect(BattleContext& context);
};

class BusterCallShip : public Building {
public:
    BusterCallShip(string name, int hp);
    void applyEffect(BattleContext& context);
    void onDestroyed(BattleContext& context);
};

/*
 * TurnNode to create Linked List turn order
 */
struct TurnNode
{
    Character* data;
    TurnNode* next;
};

/*
 * EniesLobbyBattle
 */
class EniesLobbyBattle {
private:
    Character** strawHats;
    int strawHatCount;
    Character** cp9Agents;
    int cp9Count;
    Building** buildings;
    int buildingCount;
    TurnNode* turnOrder;
    BattleContext context;
    int maxTurns;

public:
    EniesLobbyBattle(const string& filename);
    ~EniesLobbyBattle();
    void loadFromFile(const string& filename);
    void addStrawHat(Character* character);
    void addCP9Agent(Character* character);
    void addBuilding(Building* building);
    void buildTurnOrder();
    void runBattle();
    void processTurn(Character* character);
    void processBuildings();
    void checkEndCondition();
    string getResult() const;
};

#endif