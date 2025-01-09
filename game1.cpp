#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <string>
#include <cmath>
#include <iomanip> // 用於設置輸出精度
#include <unordered_map> // 用於存儲狀態效果
#include <windows.h> // 用於 Sleep() 函數
using namespace std;
//如果無法編譯 可能是環境參數跑掉了 請重新設定環境參數

// 初始化隨機數生成器
default_random_engine generator(time(0));
uniform_real_distribution<double> distribution(0.0, 1.0);

// 定義元素類型
enum Element { NONE, WATER, FIRE, WOOD };

// 怪物類別
class Monster {
private:
    int hp;
    int attack;
    Element element; // 怪物的當前屬性
    unordered_map<string, int> statusEffects; // 狀態效果存儲

public:
    Monster(int h, int atk) : hp(h), attack(atk), element(NONE) {}

    int getHP() const { return hp; }
    void setHP(int newHP) { hp = max(0, newHP); }
    int getAttack() const { return attack; }
    Element getElement() const { return element; }
    void setElement(Element elem) { element = elem; }

    // 嘗試暴擊
    bool tryCrit() const {
        static default_random_engine generator(static_cast<unsigned>(time(0)));
        static uniform_real_distribution<double> distribution(0.0, 1.0);
        return distribution(generator) < 0.25; // 25% 暴擊機率
    }

    void print() { 
        cout << "Monster HP: " << max(0, hp) << ", Attack: " << attack << ", Element: " << getElementName() << "\n"; 
    }

    // 元素名稱轉換
    string getElementName() const {
        switch (element) {
            case WATER: return "Water";
            case FIRE: return "Fire";
            case WOOD: return "Wood";
            default: return "None";
        }
    }

    void applyStatusEffect(string effect, int duration) { statusEffects[effect] = duration; } // 應用狀態效果
    // 減少狀態效果持續時間
    void decrementStatusEffects() {
        for (auto it = statusEffects.begin(); it != statusEffects.end();) {
            it->second -= 1;
            if (it->second <= 0) {
                it = statusEffects.erase(it);
            } else {
                ++it;
            }
        }
    }
};

// 基本角色類別
class Character {
protected:
    string name;
    int level; // 角色等級
    int hp, maxHP; // 血量
    int mp, maxMP; // 魔力
    int armor, maxArmor; // 護甲
    int attackPower; // 攻擊力
    int str, vit, agl, dex, intel; // 屬性值
    int skillCost = 0, elementSkillCost = 0; // 技能消耗 MP
    bool isDefending; // 是否處於防禦狀態
    Element element; // 角色元素屬性
    unordered_map<string, int> statusEffects; // 狀態效果存儲
    int exp; // 經驗值
    int expToNextLevel; // 升級所需經驗值

public:
    Character(string n, int lv, int s, int v, int ag, int d, int i, int skillC, int elementSkillC) 
        : name(n), level(lv), str(s), vit(v), agl(ag), dex(d), intel(i), exp(0), expToNextLevel(100 + lv * 5), skillCost(skillC), elementSkillCost(elementSkillC) {

        hp = 20 + vit * 20;
        mp = 50 + intel * 50;
        armor = 10 + dex * 20;
        attackPower = 10 + str * 30;
        maxArmor = armor;
        maxHP = hp;
        maxMP = mp;
        element = NONE; // 默認沒有元素
    }

    // 獲取百分比方法
    double getHPPercentage() const {
        return (static_cast<double>(hp) / maxHP) * 100;
    }

    double getMPPercentage() const {
        return (static_cast<double>(mp) / maxMP) * 100;
    }

    double getArmorPercentage() const {
        return (static_cast<double>(armor) / maxArmor) * 100.0;
    }

    virtual void print() {
        cout << fixed << setprecision(1) // 設置浮點數格式
            << name << " Lv " << level 
            << " : " << "ATK: " << attackPower 
            << " : " << "HP: " << hp << "/" << maxHP 
            << " : " << "MP: " << mp << "/" << maxMP 
            << " : " << "Armor: " << armor << "/" << maxArmor
            << " : " << "Element: " << getElementName()
            << " : " << "EXP: " << exp << "/" << expToNextLevel 
            << "\n";
        cout <<"\n";
    }
    
    virtual void useSkill(Monster& monster) = 0; // 技能接口
    virtual void useElementSkill(Character& target) = 0; // 元素技能接口
    virtual void useElementSkill(Monster& target) {
        cout << "Elemental skill used on monster!\n";
        // 處理怪物相關的技能效果
    }

    bool isAlive() const { return hp > 0; }
    int getHP() const { return hp; }
    int getMaxHP() const { return maxHP; }
    void setHP(int newHP) {
        int damage = hp - newHP;

        if (damage > 0 && isDefending) {
            if (armor > 0) {
                int reducedDamage = damage * 0.8; // 防禦減少兩成傷害
                int armorCost = damage * 0.2;    // 防禦消耗護甲值
                armor = max(0, armor - armorCost);

                cout << name << " blocked part of the damage, reducing it to " << reducedDamage 
                     << " and consuming " << armorCost << " armor points!\n";
                damage = reducedDamage;
            } else {
                cout << name << "'s armor is depleted! Defense is no longer effective.\n";
            }
        }

        hp = max(0, hp - damage);
    }

    int getMP() const { return mp; }
    int getMaxMP() const { return maxMP; }
    void setMP(int newMP) { mp = newMP; }
    int getArmor() const { return armor; }
    void setArmor(int newArmor) { armor = max(0, newArmor); }
    int getAttack() const { return attackPower; }
    int getSkillCost() const { return skillCost; } // 獲取技能消耗 MP
    int getElementSkillCost() const { return elementSkillCost; } // 獲取元素技能消耗 MP
    string getName() const { return name; }
    Element getElement() const { return element; } // 獲取元素屬性
    void setElement(Element newElement) { element = newElement; } // 設置元素屬性
    void applyStatusEffect(string effect, int duration) { statusEffects[effect] = duration; } // 應用狀態效果
    // 減少狀態效果持續時間
    void decrementStatusEffects() {
        for (auto it = statusEffects.begin(); it != statusEffects.end();) {
            it->second -= 1;
            if (it->second <= 0) {
                it = statusEffects.erase(it);
            } else {
                ++it;
            }
        }
    }

    // 嘗試閃避
    bool tryDodge(int enemyArmor) const {
        static default_random_engine generator(static_cast<unsigned>(time(0)));
        static uniform_real_distribution<double> distribution(0.0, 1.0);
        return distribution(generator) < enemyArmor / 100.0;
    }

    // 嘗試暴擊
    bool tryCrit() const {
        static default_random_engine generator(static_cast<unsigned>(time(0)));
        static uniform_real_distribution<double> distribution(0.0, 1.0);
        return distribution(generator) < dex*5 / 100.0;
    }

    bool hasStatusEffect(const string& effect) const {
        return statusEffects.find(effect) != statusEffects.end();
    }

    void combatLog(string action) {
        cout << action << "\n";
        Sleep(500); // 模擬短暫延遲，增強沉浸感
    }

    // 元素名稱轉換
    string getElementName() const {
        switch (element) {
            case WATER: return "Water";
            case FIRE: return "Fire";
            case WOOD: return "Wood";
            default: return "None";
        }
    }

    // 元素克制邏輯
    bool isEffectiveAgainst(Element targetElement) {
        return (element == WATER && targetElement == FIRE) ||
               (element == FIRE && targetElement == WOOD) ||
               (element == WOOD && targetElement == WATER);
    }

    // 獲取經驗值
    void gainExp(int amount) {
        exp += amount;
        cout << name << " gained " << amount << " EXP!\n";
        if (exp >= expToNextLevel) {
            levelUp();
        }
    }

    // 升級
    void levelUp() {
        exp -= expToNextLevel;
        level++;
        expToNextLevel = 100 + level * 5;
        hp = maxHP = static_cast<int>(maxHP * 1.1);
        mp = maxMP = static_cast<int>(maxMP * 1.1);
        attackPower = static_cast<int>(attackPower * 1.1);
        armor = maxArmor = static_cast<int>(maxArmor * 1.1);
        cout << name << " leveled up to " << level << "!\n";
    }

    void setStr(int newStr) { str = newStr; }
    void setVit(int newVit) { vit = newVit; }
    void setAgl(int newAgl) { agl = newAgl; }
    void setDex(int newDex) { dex = newDex; }
    void setIntel(int newIntel) { intel = newIntel; }
    void setYOUProperties(int newStr, int newVit, int newAgl, int newDex, int newIntel) {
        setStr(newStr);
        setVit(newVit);
        setAgl(newAgl);
        setDex(newDex);
        setIntel(newIntel);
    }

    int getStr() const { return str; }
    int getVit() const { return vit; }
    int getAgl() const { return agl; }
    int getDex() const { return dex; }
    int getIntel() const { return intel; }
};

// 技能類別
class Skill {
private:
    string name;
    int cost;
    int effect;

public:
    Skill(string n, int c, int e) : name(n), cost(c), effect(e) {}

    string getName() const { return name; }
    int getCost() const { return cost; }
    int getEffect() const { return effect; }

    void use(Character& user, Monster& target) {
        if (user.getMP() >= cost) {
            user.setMP(user.getMP() - cost);
            target.setHP(target.getHP() - effect);
            cout << user.getName() << " used " << name << "! Dealt " << effect << " damage to the monster.\n";
            cout <<"\n";
            cout << "MP remaining: " << user.getMP() << "/" << user.getMaxMP() << "\n";
            cout <<"\n";
        } else {
            cout << user.getName() << " does not have enough MP to use " << name << "!\n";
            cout <<"\n";
        }
    }
};

// 元素技能類別
class ElementSkill {
private:
    string name;
    int cost;
    int effect;
    Element element;

public:
    ElementSkill(string n, int c, int e, Element elem) : name(n), cost(c), effect(e), element(elem) {}

    string getName() const { return name; }
    int getCost() const { return cost; }
    int getEffect() const { return effect; }
    Element getElement() const { return element; }

    void use(Character& user, Monster& target) {
        if (user.getMP() >= cost) {
            user.setMP(user.getMP() - cost);
            target.setElement(element); // 覆蓋敵人的元素
            cout << user.getName() << " used " << name << "! The monster's element is now " << getElementName() << ".\n";
            cout << "\n";
            cout << "MP remaining: " << user.getMP() << "/" << user.getMaxMP() << "\n";
            cout << "\n";
        } else {
            cout << user.getName() << " does not have enough MP to use " << name << "!\n";
            cout << "\n";
        }
    }

    void use(Character& user, Character& target) {
        if (user.getMP() >= cost) {
            user.setMP(user.getMP() - cost);
            int finalEffect = effect;

            // 如果目標是隊友
            if (user.getElement() == target.getElement()) {
                finalEffect *= 2; // 相同屬性效果加倍
                target.applyStatusEffect("Attack Boost", 1); // 攻擊力加倍一回合
                cout << "Synergy! " << user.getName() << "'s " << name << " is twice as effective on " << target.getName() << "!\n";
                cout << target.getName() << "'s attack power is doubled for the next turn!\n";
                cout << "\n";
            }

            target.setHP(target.getHP() + finalEffect);
            cout << user.getName() << " used " << name << " on " << target.getName() << "! Restored " << finalEffect << " HP.\n";
            cout << "MP remaining: " << user.getMP() << "/" << user.getMaxMP() << "\n";
            cout << "\n";
        } else {
            cout << user.getName() << " does not have enough MP to use " << name << "!\n";
        }
    }

    string getElementName() const {
        switch (element) {
            case WATER: return "Water";
            case FIRE: return "Fire";
            case WOOD: return "Wood";
            default: return "None";
        }
    }
};

// 戰士類別
class Warrior : public Character {
private:
    Skill skill;
    ElementSkill elementSkill;

public:
    Warrior(string n, int lv, int s, int v, int ag, int d, int i)
        : Character(n, lv, s, v, ag, d, i, 10, 20), skill("Power Strike", 10, 40), elementSkill("Flame Slash", 20, 50, FIRE) {
        element = FIRE;
    }

    void print() override {
        cout << "Warrior: ";
        Character::print();
    }

    void useSkill(Monster& monster) override {
        if (getMP() >= skill.getCost()) {
            setMP(getMP() - skill.getCost());
            int damage = getAttack() * 1.5;
            monster.setHP(monster.getHP() - damage);
            cout << getName() << " used Power Strike! Dealt " << damage << " damage to the monster.\n";
            cout << "\n";
        } else {
            cout << "Not enough MP to use Power Strike.\n";
        }
    }

    void useElementSkill(Monster& target) override {
        elementSkill.use(*this, target);
    }

    void useElementSkill(Character& target) override {
        elementSkill.use(*this, target);
    }
};

// 巫師類別
class Wizard : public Character {
private:
    Skill skill;
    ElementSkill elementSkill;

public:
    Wizard(string n, int lv, int s, int v, int ag, int d, int i)
        : Character(n, lv, s, v, ag, d, i, 15, 30), skill("Arcane Shield", 15, 0), elementSkill("Water Blast", 30, 60, WATER) {
        element = WATER;
    }

    void print() override {
        cout << "Wizard: ";
        Character::print();
    }

    void useSkill(Monster& monster) override {
        if (getMP() >= skill.getCost()) {
            setMP(getMP() - skill.getCost());
            applyStatusEffect("Invincible", 1);
            cout << getName() << " used Arcane Shield! Will ignore the next attack.\n";
            cout << "\n";
        } else {
            cout << "Not enough MP to use Arcane Shield.\n";
        }
    }

    void useElementSkill(Monster& target) override {
        elementSkill.use(*this, target);
    }

    void useElementSkill(Character& target) override {
        elementSkill.use(*this, target);
    }
};

// 坦克類別
class Tank : public Character {
private:
    Skill skill;
    ElementSkill elementSkill;

public:
    Tank(string n, int lv, int s, int v, int ag, int d, int i)
        : Character(n, lv, s, v, ag, d, i, 5, 10), skill("Fortify", 5, 0), elementSkill("Wooden Guard", 10, 30, WOOD) {
        element = WOOD;
    }

    void print() override {
        cout << "Tank: ";
        Character::print();
    }

    void useSkill(Monster& monster) override {
        if (getMP() >= skill.getCost()) {
            setMP(getMP() - skill.getCost());
            setArmor(getArmor() + 30);
            cout << getName() << " used Fortify! Increased armor by 30.\n";
            cout << "\n";
        } else {
            cout << "Not enough MP to use Fortify.\n";
        }
    }

    void useElementSkill(Monster& target) override {
        elementSkill.use(*this, target);
    }

    void useElementSkill(Character& target) override {
        elementSkill.use(*this, target);
    }
};

// 精靈類別
class Elf : public Character {
private:
    Skill skill;
    ElementSkill elementSkill;

public:
    Elf(string n, int lv, int s, int v, int ag, int d, int i)
        : Character(n, lv, s, v, ag, d, i, 15, 30), skill("Arrow Barrage", 15, 20), elementSkill("Nature's Wrath", 30, 60, WOOD) {
        element = WOOD;
    }

    void print() override {
        cout << "Elf: ";
        Character::print();
    }

    void useSkill(Monster& monster) override {
        if (getMP() >= skill.getCost()) {
            setMP(getMP() - skill.getCost());
            int damage = getAttack() * 1.1;
            monster.setHP(monster.getHP() - damage);
            cout << getName() << " used Arrow Barrage! Dealt " << damage << " damage to the monster.\n";
            cout << "\n";
        } else {
            cout << "Not enough MP to use Arrow Barrage.\n";
        }
    }

    void useElementSkill(Monster& target) override {
        elementSkill.use(*this, target);
    }

    void useElementSkill(Character& target) override {
        elementSkill.use(*this, target);
    }
};

// 治療者類別
class Healer : public Character {
private:
    Skill skill;
    ElementSkill elementSkill;

public:
    Healer(string n, int lv, int s, int v, int ag, int d, int i)
        : Character(n, lv, s, v, ag, d, i, 20, 40), skill("Healing Light", 20, 25), elementSkill("Holy Water", 40, 50, WATER) {
        element = WATER;
    }

    void print() override {
        cout << "Healer: ";
        Character::print();
    }

    void useSkill(Monster& monster) override {
        if (getMP() >= skill.getCost()) {
            setMP(getMP() - skill.getCost());
            setHP(getHP() + 25);
            cout << getName() << " used Healing Light! Restored 25 HP.\n";
            cout << "\n";
        } else {
            cout << "Not enough MP to use Healing Light.\n";
        }
    }

    void useElementSkill(Monster& target) override {
        elementSkill.use(*this, target);
    }

    void useElementSkill(Character& target) override {
        elementSkill.use(*this, target);
    }
};

// 哥布林類別
class Goblin : public Character {
private:
    Skill skill;
    ElementSkill elementSkill;

public:
    Goblin(string n, int lv, int s, int v, int ag, int d, int i)
        : Character(n, lv, s, v, ag, d, i, 10, 20), skill("Sneak Attack", 10, 30), elementSkill("Poison Dart", 20, 40, WOOD) {
        element = WOOD;
    }

    void print() override {
        cout << "Goblin: ";
        Character::print();
    }

    void useSkill(Monster& monster) override {
        if (getMP() >= skill.getCost()) {
            setMP(getMP() - skill.getCost());
            int damage = getAttack() * 1.3;
            monster.setHP(monster.getHP() - damage);
            cout << getName() << " used Sneak Attack! Dealt " << damage << " damage to the monster.\n";
            cout << "\n";
        } else {
            cout << "Not enough MP to use Sneak Attack.\n";
        }
    }

    void useElementSkill(Monster& target) override {
        elementSkill.use(*this, target);
    }

    void useElementSkill(Character& target) override {
        elementSkill.use(*this, target);
    }
};

// 騎士類別
class Knight : public Character {
private:
    Skill skill;
    ElementSkill elementSkill;

public:
    Knight(string n, int lv, int s, int v, int ag, int d, int i)
        : Character(n, lv, s, v, ag, d, i, 15, 30), skill("Shield Charge", 15, 25), elementSkill("Fire Lance", 30, 60, FIRE) {
        element = FIRE;
    }

    void print() override {
        cout << "Knight: ";
        Character::print();
    }

    void useSkill(Monster& monster) override {
        if (getMP() >= skill.getCost()) {
            setMP(getMP() - skill.getCost());
            int damage = getAttack() * 1.4;
            monster.setHP(monster.getHP() - damage);
            cout << getName() << " used Shield Charge! Dealt " << damage << " damage to the monster.\n";
            cout << "\n";
        } else {
            cout << "Not enough MP to use Shield Charge.\n";
        }
    }

    void useElementSkill(Monster& target) override {
        elementSkill.use(*this, target);
    }

    void useElementSkill(Character& target) override {
        elementSkill.use(*this, target);
    }
};

class YOU : public Character {
private:
    Skill skill;
    ElementSkill elementSkill; // 元素技能

public:
    YOU(string n, int lv, int s, int v, int ag, int d, int i)
        : Character(n, lv, s, v, ag, d, i, 10, 20), skill("Power Strike", 10, 40), elementSkill("Flame Slash", 20, 50, NONE) {
        element = NONE;
    }

    void print() override {
        cout << "YOU: ";
        Character::print();
    }
    void useSkill(Monster& monster) override {
        cout << getName() << " not have skill\n";
        cout << "\n";
    }

    void useElementSkill(Monster& target) override {
        if (element == NONE) {
            cout << getName() << " does not have an elemental skill.\n";
        } else {
            elementSkill.use(*this, target);
        }
    }

    void useElementSkill(Character& target) override {
        if (element == NONE) {
            cout << getName() << " does not have an elemental skill.\n";
        } else {
            elementSkill.use(*this, target);
        }
    }

    void setElementSkill(ElementSkill newElementSkill) {
        elementSkill = newElementSkill;
    }
};

class teacher : public Character {
private:
    Skill skill;
    ElementSkill elementSkill; // 元素技能

public:
    teacher(string n, int lv, int s, int v, int ag, int d, int i)
        : Character(n, lv, s, v, ag, d, i, 0, 20), skill("Cheater Attack!!", 0, 0), elementSkill("Flame Slash", 20, 50, NONE) {
        element = NONE;
    }

    void print() override {
        cout << "teacher: ";
        Character::print();
    }

    void useSkill(Monster& monster) override {
        if (getMP() >= skill.getCost()) {
            setMP(getMP() - skill.getCost());
            int damage = getAttack() + 10000 ;
            monster.setHP(monster.getHP() - damage);
            cout << getName() << " used I AM CHEATER!! " << damage << " damage to the monster.\n";
            cout << "\n";
        } else {
            cout << "Not enough MP to use Shield Charge.\n";
        }
    }

    void useElementSkill(Monster& target) override {
        elementSkill.use(*this, target);
    }

    void useElementSkill(Character& target) override {
        elementSkill.use(*this, target);
    }
};

// 團隊類別
class Team {
private:
    vector<Character*> members;
    int coin; // 新增 coin 屬性

public:
    Team() : coin(0) {} // 初始化 coin 為 0

    ~Team() {
        for (auto member : members) {
            delete member;
        }
        members.clear();
    }

    void addCharacter(Character* character) {
        if (character) {
            members.push_back(character);
        } else {
            cout << "Invalid character.\n";
        }
    }

    vector<Character*> getMembers() const {
        return members;
    }

    void print() const {
        for (auto member : members) {
            member->print();
        }
    }

    void addCoin(int amount) {
        coin += amount;
        cout << "The team gained " << amount << " coins! Total coins: " << coin << "\n";
        cout << "\n";
    }

    void spendCoin(int amount) {
        if (coin >= amount) {
            coin -= amount;
            cout << "Spent " << amount << " coins. Remaining coins: " << coin << "\n";
        } else {
            cout << "Not enough coins.\n";
        }
    }

    int getCoin() const {
        return coin;
    }

    void combat(Monster& monster) {
        while (true) {
            // 顯示怪物狀態
            cout << "\nMonster's Status:\n";
            monster.print();

            // 玩家回合
            for (size_t turn = 0; turn < members.size(); turn++) {
                cout << "\nSelect a team member to take action:\n";
                cout << "\n";
                for (size_t i = 0; i < members.size(); i++) {
                    cout << i + 1 << ". ";
                    members[i]->print(); // 顯示角色的狀態
                }

                int choice;
                cout << "Choose a team member (1-" << members.size() << "): ";
                cin >> choice;

                if (choice < 1 || choice > members.size() || !members[choice - 1]->isAlive()) {
                    cout << "Invalid choice or character is not alive. Turn skipped.\n";
                    cout <<"\n";
                    continue;
                }

                Character* member = members[choice - 1];
                cout << "\nIt's " << member->getName() << "'s turn.\n";

                // 選擇行動
                cout << "1. Normal Attack\n2. Use Skill\n3. Use Element Skill\n4. Skip Turn\nChoose an action: ";
                cout << "\n";
                int action;
                cin >> action;

                switch (action) {
                    case 1: {
                        // 普通攻擊
                        bool crit = member->tryCrit();
                        int damage = crit ? member->getAttack() * 2 : member->getAttack();
                        if (member->isEffectiveAgainst(monster.getElement())) {
                            damage *= 2; // 如果相剋，傷害加倍
                            cout << "Super effective! ";
                        }
                        if (member->hasStatusEffect("Attack Boost")) {
                            damage *= 2; // 攻擊力加倍效果
                            cout << "Attack Boost! ";
                        }
                        cout << member->getName() << " attacked the monster, dealing " << damage << (crit ? " (Critical Hit!)" : "") << " damage!\n";
                        cout << "\n";
                        monster.setHP(monster.getHP() - damage);
                        monster.print(); // 顯示怪物狀態
                        break;
                    }
                    case 2: {
                        // 使用技能
                        if (member->getMP() >= member->getSkillCost()) {
                            member->useSkill(monster);
                            monster.print(); // 顯示怪物狀態
                        } else {
                            cout << "Not enough MP to use skill.\n";
                            cout << "\n";
                        }
                        break;
                    }
                    case 3: {
                        // 使用元素技能
                        cout << "1. Use Element Skill on Monster\n2. Use Element Skill on a Teammate\nChoose a target: ";
                        cout << "\n";
                        int targetAction;
                        cin >> targetAction;

                        if (targetAction == 1) {
                            // 對怪物使用元素技能
                            member->useElementSkill(monster);
                            monster.print(); // 顯示怪物狀態
                        } else if (targetAction == 2) {
                            // 對隊友使用元素技能
                            vector<Character*> aliveTeammates;
                            cout << "Choose a teammate to target for Elemental Blessing:\n";
                            for (size_t i = 0; i < members.size(); i++) {
                                if (members[i] != member && members[i]->isAlive()) {
                                    aliveTeammates.push_back(members[i]);
                                    cout << i + 1 << ". " << members[i]->getName() << "\n";
                                }
                            }

                            if (aliveTeammates.empty()) {
                                cout << "No living teammates available for Elemental Blessing.\n";
                            } else {
                                int targetChoice;
                                cout << "Choose a teammate (1-" << aliveTeammates.size() << "): ";
                                cin >> targetChoice;

                                if (targetChoice < 1 || targetChoice > aliveTeammates.size()) {
                                    cout << "Invalid selection.\n";
                                } else {
                                    // 使用元素技能
                                    member->useElementSkill(*aliveTeammates[targetChoice - 1]);
                                }
                            }
                        }
                        break;
                    }
                    case 4:
                        cout << member->getName() << " skipped their turn.\n";
                        break;
                    default:
                        cout << "Invalid action. Turn skipped.\n";
                        break;
                }

                // 如果怪物被擊敗，結束戰鬥
                if (monster.getHP() <= 0) {
                    cout << "\nThe team has defeated the monster!\n";
                    cout << "Victory!\n";
                    cout << "\n";
                    for (auto& member : members) {
                        member->gainExp(1000); // 每個成員獲得經驗值
                    }
                    addCoin(100); // 獲得 100 個 coin
                    return;
                }
            }

            // 怪物回合
            cout << "\nMonster's turn!\n";
            cout << "\n";
            for (auto& member : members) {
                if (member->isAlive()) {
                    // 怪物攻擊
                    if (monster.tryCrit()) {
                        int damage = monster.getAttack() * 2;
                        member->setHP(member->getHP() - damage);
                        cout << "The monster critically hit " << member->getName() << ", dealing " << damage << " damage!\n";
                    } else if (!member->tryDodge(member->getArmor())) {
                        int damage = max(1, monster.getAttack());
                        member->setHP(member->getHP() - damage);
                        cout << "The monster attacked " << member->getName() << ", dealing " << damage << " damage.\n";
                    } else {
                        cout << member->getName() << " dodged the monster's attack!\n";
                    }

                    if (!member->isAlive()) {
                        cout << member->getName() << " has fallen!\n";
                        if (member->getName() == "YOU") {
                            cout << "YOU have fallen! GAME OVER\n";
                            exit(0); // 程式結束
                        }
                    }
                }
            }

            // 如果全隊成員都死了，結束戰鬥
            if (all_of(members.begin(), members.end(), [](Character* c) { return !c->isAlive(); })) {
                cout << "\nThe team has been defeated...\n";
                cout << "GAME OVER\n";
                return;
            }
        }
    }
};

void randomCharacter(Team& team) {
    // 隨機生成角色屬性
    uniform_int_distribution<int> dist(1, 7);
    int role = dist(generator);
    string name;
    int s, v, ag, d, i;

    cout << "Enter character details (name, str, vit, agl, dex, intel): " << "\n";
    cout << "The total attribute points cannot exceed 15." << "\n"; 
    cout << "And the maximum value for each attribute is 10." << "\n";
    while (true) {
        // 輸入角色資訊
        cout << "Name: ";
        cin >> ws; // 清除輸入緩衝區中的空白符號
        getline(cin, name);
        
        cout << "Attributes (str vit agl dex intel): ";
        cin >> s >> v >> ag >> d >> i;

        // 檢查輸入條件是否符合
        if (name.empty()) {
            cout << "Name cannot be empty. Please try again." << "\n";
        } else if (s + v + ag + d + i > 15) {
            cout << "Total attribute points exceed 15. Please try again." << "\n";
        } else if (s > 10 || v > 10 || ag > 10 || d > 10 || i > 10) {
            cout << "Each attribute must be 10 or less. Please try again." << "\n";
        } else if (s < 0 || v < 0 || ag < 0 || d < 0 || i < 0) {
            cout << "Attributes cannot be negative. Please try again." << "\n";
        } else {
            // 成功輸入
            break;
        }

        // 如果有輸入錯誤，清除錯誤狀態並清空緩衝區
        if (cin.fail()) {
            cin.clear(); // 清除錯誤狀態
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略緩衝區內容
        }
    }

    // 設置角色等級
    int level = 3;

    // 創建角色並確保屬性分配符合條件
    Character* newCharacter = nullptr;
    if (role == 1) {
        newCharacter = new Warrior(name, level, s, v, ag, d, i);
        cout << "A warrior " << name << " has become your teammate." << "\n";
        cout << "Skills: Power Strike, Flame Slash\n";
        cout << "Element: FIRE\n";
        cout << "Skill Points: STR: " << s << ", VIT: " << v << ", AGL: " << ag << ", DEX: " << d << ", INT: " << i << "\n";
        cout << "\n";
    } 
    else if (role == 2) {
        newCharacter = new Wizard(name, level, s, v, ag, d, i);
        cout << "A wizard " << name << " has become your teammate." << "\n";
        cout << "Skills: Fireball, Ice Shard\n";
        cout << "Element: WATER\n";
        cout << "Skill Points: STR: " << s << ", VIT: " << v << ", AGL: " << ag << ", DEX: " << d << ", INT: " << i << "\n";
        cout << "\n";
    }
    else if (role == 3) {
        newCharacter = new Tank(name, level, s, v, ag, d, i);
        cout << "A tank " << name << " has become your teammate." << "\n";
        cout << "Skills: Shield Bash, Fortress Stance\n";
        cout << "Element: WOOD\n";
        cout << "Skill Points: STR: " << s << ", VIT: " << v << ", AGL: " << ag << ", DEX: " << d << ", INT: " << i << "\n";
        cout << "\n";
    } 
    else if (role == 4) {
        newCharacter = new Elf(name, level, s, v, ag, d, i);
        cout << "An elf " << name << " has become your teammate." << "\n";
        cout << "Skills: Arrow Volley, Nature's Wrath\n";
        cout << "Element: WOOD\n";
        cout << "Skill Points: STR: " << s << ", VIT: " << v << ", AGL: " << ag << ", DEX: " << d << ", INT: " << i << "\n";
        cout << "\n";
    }
    else if (role == 5) {
        newCharacter = new Healer(name, level, s, v, ag, d, i);
        cout << "A healer " << name << " has become your teammate." << "\n";
        cout << "Skills: Healing Light, Revive\n";
        cout << "Element: WATER\n";
        cout << "Skill Points: STR: " << s << ", VIT: " << v << ", AGL: " << ag << ", DEX: " << d << ", INT: " << i << "\n";
        cout << "\n";
    }
    else if (role == 6) {
        newCharacter = new Goblin(name, level, s, v, ag, d, i);
        cout << "A goblin " << name << " has become your teammate." << "\n";
        cout << "Skills: Sneak Attack, Poison Dagger\n";
        cout << "Element: WOOD\n";
        cout << "Skill Points: STR: " << s << ", VIT: " << v << ", AGL: " << ag << ", DEX: " << d << ", INT: " << i << "\n";
        cout << "\n";
    }
    else if (role == 7) {
        newCharacter = new Knight(name, level, s, v, ag, d, i);
        cout << "A knight " << name << " has become your teammate." << "\n";
        cout << "Skills: Holy Blade, Shield Charge\n";
        cout << "Element: FIRE\n";
        cout << "Skill Points: STR: " << s << ", VIT: " << v << ", AGL: " << ag << ", DEX: " << d << ", INT: " << i << "\n";
        cout << "\n";
    }

    if (newCharacter) {
        team.addCharacter(newCharacter);
    }
}

void setYOUElement(Team& team, Element newElement) {
    for (auto& member : team.getMembers()) {
        if (member->getName() == "YOU") {
            member->setElement(newElement);
            cout << "YOU's element has been changed to " << member->getElementName() << "!\n";
            return;
        }
    }
    cout << "YOU character not found.\n";
}

void setYOUProperties(Team& team, int newStr, int newVit, int newAgl, int newDex, int newIntel) {
    for (auto& member : team.getMembers()) {
        if (member->getName() == "YOU") {
            member->setYOUProperties(newStr, newVit, newAgl, newDex, newIntel);
            cout << "YOU's properties have been changed!\n";
            return;
        }
    }
    cout << "YOU character not found.\n";
}

void store(Team& team) {
    while (true) {
        cout << "Welcome to the store!" << "\n";
        cout << "You have " << team.getCoin() << " coins." << "\n";
        cout << "1. Buy a STR point (50 coins)\n";
        cout << "2. Buy a VIT point (50 coins)\n";
        cout << "3. Buy a AGL point (50 coins)\n";
        cout << "4. Buy a DEX point (50 coins)\n";
        cout << "5. Buy a INT point (50 coins)\n";
        cout << "6. Buy an element of WOOD (200 coins)\n";
        cout << "7. Buy an element of FIRE (200 coins)\n";
        cout << "8. Buy an element of WATER (200 coins)\n";
        cout << "9. Exit\n";
        cout << "Choose an option: ";
        int choice;
        cin >> choice;
        cout << "\n";

        if (choice == 9) {
            cout << "Exiting store...\n";
            break;
        }

        Character* member = nullptr;
        for (auto& m : team.getMembers()) {
            if (m->getName() == "YOU") {
                member = m;
                break;
            }
        }

        if (!member) {
            cout << "YOU character not found.\n";
            continue;
        }

        switch (choice) {
            case 1:
                if (team.getCoin() >= 50) {
                    setYOUProperties(team, member->getStr() + 1, member->getVit(), member->getAgl(), member->getDex(), member->getIntel());
                    team.spendCoin(50);
                    cout << member->getName() << "'s STR increased by 1!\n";
                } else {
                    cout << "Not enough coins.\n";
                }
                break;
            case 2:
                if (team.getCoin() >= 50) {
                    setYOUProperties(team, member->getStr(), member->getVit() + 1, member->getAgl(), member->getDex(), member->getIntel());
                    team.spendCoin(50);
                    cout << member->getName() << "'s VIT increased by 1!\n";
                } else {
                    cout << "Not enough coins.\n";
                }
                break;
            case 3:
                if (team.getCoin() >= 50) {
                    setYOUProperties(team, member->getStr(), member->getVit(), member->getAgl() + 1, member->getDex(), member->getIntel());
                    team.spendCoin(50);
                    cout << member->getName() << "'s AGL increased by 1!\n";
                } else {
                    cout << "Not enough coins.\n";
                }
                break;
            case 4:
                if (team.getCoin() >= 50) {
                    setYOUProperties(team, member->getStr(), member->getVit(), member->getAgl(), member->getDex() + 1, member->getIntel());
                    team.spendCoin(50);
                    cout << member->getName() << "'s DEX increased by 1!\n";
                } else {
                    cout << "Not enough coins.\n";
                }
                break;
            case 5:
                if (team.getCoin() >= 50) {
                    setYOUProperties(team, member->getStr(), member->getVit(), member->getAgl(), member->getDex(), member->getIntel() + 1);
                    team.spendCoin(50);
                    cout << member->getName() << "'s INT increased by 1!\n";
                } else {
                    cout << "Not enough coins.\n";
                }
                break;
            case 6:
                if (team.getCoin() >= 200) {
                    setYOUElement(team, WOOD);
                    team.spendCoin(200);
                    dynamic_cast<YOU*>(member)->setElementSkill(ElementSkill("Wooden Guard", 20, 50, WOOD));
                } else {
                    cout << "Not enough coins.\n";
                }
                break;
            case 7:
                if (team.getCoin() >= 200) {
                    setYOUElement(team, FIRE);
                    team.spendCoin(200);
                    dynamic_cast<YOU*>(member)->setElementSkill(ElementSkill("Flame Slash", 20, 50, FIRE));
                } else {
                    cout << "Not enough coins.\n";
                }
                break;
            case 8:
                if (team.getCoin() >= 200) {
                    setYOUElement(team, WATER);
                    team.spendCoin(200);
                    dynamic_cast<YOU*>(member)->setElementSkill(ElementSkill("Water Blast", 20, 50, WATER));
                } else {
                    cout << "Not enough coins.\n";
                }
                break;
            default:
                cout << "Invalid choice.\n";
                break;
        }
        cout << "\n";
    }
}

int getValidChoice() {
    string choice;
    while (true) {
        cin >> choice;
        transform(choice.begin(), choice.end(), choice.begin(), ::toupper);
        if (choice == "Y" || choice == "N") {
            return choice == "Y" ? 1 : 0;
        } else {
            cout << "Invalid input. Please enter 'Y' or 'N'." << "\n";
        }
    }
}

// 主程序
int main() {
    while (true) {
        Team team; // 創建 Team 實例

        int mode;
        while (true) {
            cout << "1. solo mode" << "\n";
            cout << "2. team mode" << "\n";
            cout << "choose solo or team mode: ";
            cin >> mode;
            if (mode == 1 || mode == 2) {
                break;
            } else {
                cout << "Invalid choice. Please try again." << "\n";
            }
        }

        cout << "my random game" << "\n";
        Sleep(1000);  // 延遲 1000 毫秒（1 秒）
        cout << "random game" << "\n";
        Sleep(1000);  // 延遲 1000 毫秒（1 秒）
        cout << "GAME";
        Sleep(300);  // 延遲 1000 毫秒（1 秒）
        for(int i = 0; i < 3; i++){
            cout << ".";
            Sleep(500);  // 延遲 1000 毫秒（1 秒）
        }
        cout << "START!!!" << "\n";
        cout << "\n";
        Sleep(1000);
        cout << "You are a student from Taipei Tech, and on your way back to the dorm after school," << "\n";
        cout << "\n";
        Sleep(2500);
        cout << "You were hit and killed by a truck because you didn't pay attention to the traffic lights." << "\n";
        cout << "\n";
        Sleep(2500);
        cout << "The GOD how unfortunate your situation was, a deity decided to send you to another world." << "\n";
        cout << "\n";
        Sleep(2500);

        mt19937 rng(static_cast<unsigned int>(time(0))); // Mersenne Twister 隨機數生成器
        uniform_int_distribution<int> dist(1, 3); // 生成範圍 [1, 3] 的隨機數

        int coin =0;
        int runAway = 0;
        int randomNumber = 1;
        string choice; // 宣告 choice 變數
        if(randomNumber == 1){
            team.addCharacter(new YOU("YOU", 1, 4, 3, 2, 4, 2));
            cout << "Now you have been transported to an ancient world filled with monsters." <<"\n";
            cout << "\n";
            Sleep(2500);
            cout << "Since you are an ordinary person. " << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "You have no skills, no elemental affinity." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "And all your skill points start from scratch." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "You must defeat the boss of this world to regain your life." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "Then you met someone on the road." << "\n";
            cout << "\n";
            Sleep(2500);

            if(mode == 1){
                cout << "He sees you as a chill guy, so he decides to become your good friend." << "\n";
                cout << "\n";
                Sleep(2500);
            }
            else{
                cout << "Who said they also wanted to defeat the boss." << "\n";
                cout << "\n";
                randomCharacter(team);
                Sleep(2500);
            }

            cout << "After that, you became good friends with them." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "And went on adventures together." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "Until you encountered the boss's first subordinate." << "\n";
            cout << "\n";
            Sleep(2500);
   
            cout << "Do you want to engage in battle? (Y/N): ";
            if(getValidChoice()) {
                Monster monster(500, 15);
                cout << "Monster appeared!" << "\n";
                cout << "\n";
                Sleep(2500);
                cout << "The battle begins!" << "\n";
                cout << "\n";
                Sleep(2500);
                team.combat(monster);
            } else {
                runAway++;
            }

            cout << "After that, you traveled together to a ruins." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "Where you met a young man who had been living ther." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "And resisting the boss's subordinates for many years." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "He said he would help you defeat the boss." << "\n";
            cout << "\n";
            Sleep(2500);
            
            if(mode == 1){
                cout << "He sees you as a chill guy, so he decides to become your good friend." << "\n";
                cout << "\n";
                Sleep(2500);
            }
            else{
                cout << "Who said they also wanted to defeat the boss." << "\n";
                cout << "\n";
                randomCharacter(team);
                Sleep(2500);
            }
            
            cout << "And so, we headed towards the castle of the second subordinate." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "Until we encountered him..." << "\n";
            cout << "\n";
            Sleep(2500);

            cout << "Do you want to engage in battle? (Y/N): ";
            if(getValidChoice()) {
                Monster monster(1000, 30);
                cout << "Monster appeared!" << "\n";
                cout << "\n";
                Sleep(2500);
                cout << "The battle begins!" << "\n";
                cout << "\n";
                Sleep(2500);
                team.combat(monster);
            } else {
                runAway++;
            }

            cout <<"After leaving the castle and going through days of travel." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "You accumulated many connections. " << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "Among them was a like-minded friend." << "\n"; 
            cout << "\n";
            Sleep(2500);

            if(mode == 1){
                cout << "He sees you as a chill guy, so he decides to become your good friend." << "\n";
                cout << "\n";
                Sleep(2500);
            }
            else{
                cout << "Who said they also wanted to defeat the boss." << "\n";
                cout << "\n";
                randomCharacter(team);
                Sleep(2500);
            }
            cout << "Who also expressed a desire to protect world peace." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "And so, we headed towards the castle of the third subordinate." << "\n";
            cout << "\n";
            Sleep(2500);

            cout << "Do you want to engage in battle? (Y/N): ";
            if(getValidChoice()) {
                Monster monster(3000, 10);
                cout << "Monster appeared! " << "\n";
                cout << "\n";
                Sleep(2500);
                cout << "The battle begins!" << "\n";
                cout << "\n";
                Sleep(2500);
                team.combat(monster);
            } else {
                runAway++;
            }

            cout << "Later, you saw the hero trapped in a cage." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "You rescued him, and in gratitude." << "\n"; 
            cout << "\n";
            Sleep(2500);
            if(mode == 1){
                cout << "He sees you as a chill guy, so he decides to become your good friend." << "\n";
                cout << "\n";
                Sleep(2500);
            }
            else{
                cout << "Who said they also wanted to defeat the boss." << "\n";
                cout << "\n";
                randomCharacter(team);
                Sleep(2500);
            }
            cout << "But we need to buy some support" << "\n";
            cout << "\n";
            Sleep(2500);
            store(team); // 添加商店調用
            cout << "After buying supplies." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "We set off for the Demon King's castle." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "And finally encountered the Demon King." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "The Demon King said." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "Since you were able to defeat my subordinates, come and face me!" << "\n";
            cout << "\n";
            Sleep(2500);

            Monster monster(5000, 25);
            cout << "Final boss appeared! " << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "The battle begins!" << "\n";
            cout << "\n";
            Sleep(2500);
            team.combat(monster);
            cout << "After that, you defeated the boss." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "And a portal appeared before you." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "You touched it, only to realize that it was all just a dream. :D" << "\n";
        }

        if(randomNumber == 2){
            team.addCharacter(new YOU("YOU", 1, 4, 3, 2, 4, 2));
            cout << "Now you have been transported to a neon-lit cyberpunk world filled with rogue AI and cyborgs." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "You are just an average hacker with limited skills." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "Your goal: stop the rogue AI threatening the city." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "Along the way, you meet a skilled cyborg who offers to assist you." << "\n";
            cout << "\n";

            if(mode == 1){
                cout << "He sees you as a chill guy, so he decides to become your good friend." << "\n";
                cout << "\n";
                Sleep(2500);
            }
            else{
            cout << "Who said they also wanted to defeat the rogue AI." << "\n";
            cout << "\n";
            randomCharacter(team);
            Sleep(2500);
            }

            // 第一次小怪遭遇
            cout << "While exploring the city, you encounter a low-level rogue drone." << "\n";
            cout << "\n";
            Sleep(2500);

            cout << "Do you want to engage in battle? (Y/N): ";
            if(getValidChoice()) {
                Monster drone(700, 15);
                cout << "Rogue Drone appeared!" << "\n";
                cout << "\n";
                Sleep(2500);
                cout << "The battle begins!" << "\n";
                cout << "\n";
                Sleep(2500);
                team.combat(drone);
            } else {
                runAway++;
            }

            // 第二次小怪遭遇
            cout << "After you defeated him, you obtained a support robot." << "\n";
            cout << "\n";
            Sleep(2500);
            if(mode == 1){
                cout << "He sees you as a chill guy, so he decides to become your good friend." << "\n";
                cout << "\n";
                Sleep(2500);
            }
            else{
            cout << "Who said they also wanted to defeat the rogue AI." << "\n";
            cout << "\n";
            randomCharacter(team);
            Sleep(2500);
            }
            
            cout << "As you progress, a mid-level security bot intercepts you." << "\n";
            cout << "\n";
            Sleep(2500);

            cout << "Do you want to engage in battle? (Y/N): ";
            if(getValidChoice()) {
                Monster securityBot(1500, 20);
                cout << "Security Bot appeared!" << "\n";
                cout << "\n";
                Sleep(2500);
                cout << "The battle begins!" << "\n";
                cout << "\n";
                Sleep(2500);
                team.combat(securityBot);
            } else {
                runAway++;
            }

            // 第三次小怪遭遇
            cout << "Near the AI's mainframe, you encounter an elite cyborg assassin." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "At this moment, you notice someone wandering nearby." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "You go over to take a look and realize it s a person." << "\n";
            cout << "\n";
            Sleep(2500);
            if(mode == 1){
                cout << "He sees you as a chill guy, so he decides to become your good friend." << "\n";
                cout << "\n";
                Sleep(2500);
            }
            else{
            cout << "Who said they also wanted to defeat the rogue AI." << "\n";
            cout << "\n";
            randomCharacter(team);
            Sleep(2500);
            }

            cout << "Do you want to engage in battle? (Y/N): ";
            if(getValidChoice()) {
                Monster assassin(3000, 25);
                cout << "Cyborg Assassin appeared!" << "\n";
                cout << "\n";
                Sleep(2500);
                cout << "The battle begins!" << "\n";
                cout << "\n";
                Sleep(2500);
                team.combat(assassin);
            } else {
                runAway++;
            }

            // 商店購買
            cout << "After the battles, you find a black-market shop selling weapons and programs." << "\n";
            cout << "\n";
            Sleep(2500);
            store(team);

            // BOSS戰
            cout << "Fully equipped, you infiltrate the AI's core system and encounter the rogue AI itself." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "The AI generates a massive combat mech to stop you." << "\n";
            cout << "\n";
            Sleep(2500);

            if(runAway == 3){
                cout << "At this moment, the GOD couldn't stand it anymore." << "\n"; 
                cout << "\n";
                Sleep(2500);
                cout << "so they decided to summon a mysterious person to help you. " << "\n";
                cout << "\n";
                Sleep(2500);

                cout << "(Y/N)?: ";
                if(getValidChoice()) {
                    team.addCharacter(new teacher("THE C++ PRPROGRAMING TEACHER", 100000000, 4, 3, 2, 4, 2));
                    cout << "His skill vera strong!!" << "\n";
                }
            }
            Monster boss(5500, 40);
            cout << "Final Boss Mech appeared!" << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "The battle begins!" << "\n";
            cout << "\n";
            Sleep(2500);
            team.combat(boss);

            cout << "You successfully shut down the rogue AI and save the city." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "But you wake up to realize it was a VR simulation! :D" << "\n";
        }

        if(randomNumber == 3){
            team.addCharacter(new YOU("YOU", 1, 4, 3, 2, 4, 2));
            cout << "Now you have been transported to a pixelated 2D world filled with monsters and treasures." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "As a simple character, your journey begins with basic tools." << "\n";            
            cout << "\n";
            Sleep(2500);
            cout << "Your mission: defeat the final boss and save the princess." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "A villager offers to join your quest." << "\n";
            cout << "\n";
            if(mode == 1){
                cout << "He sees you as a chill guy, so he decides to become your good friend." << "\n";
                cout << "\n";
                Sleep(2500);
            }
            else{
                cout << "Who said they also wanted to defeat the boss." << "\n";
                cout << "\n";
                randomCharacter(team);
                Sleep(2500);
            }

            // 第一次小怪遭遇
            cout << "You encounter a wild pixelated slime on the plains." << "\n";
            cout << "\n";
            Sleep(2500);

            cout << "Do you want to engage in battle? (Y/N): ";
            if(getValidChoice()) {
                Monster slime(600, 10);
                cout << "Pixelated Slime appeared!" << "\n";
                cout << "\n";
                Sleep(2500);
                cout << "The battle begins!" << "\n";
                cout << "\n";
                Sleep(2500);
                team.combat(slime);
            } else {
                runAway++;
            }

            // 第二次小怪遭遇
            cout << "While crossing a forest, a group of rogue goblins ambushes you." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "At that moment, a hunter lying in ambush nearby appeared." << "\n";
            cout << "\n";
            Sleep(2500);
            if(mode == 1){
                cout << "He sees you as a chill guy, so he decides to become your good friend." << "\n";
                cout << "\n";
                Sleep(2500);
            }
            else{
                cout << "Who said they also wanted to defeat the boss." << "\n";
                cout << "\n";
                randomCharacter(team);
                Sleep(2500);
            }

            cout << "Do you want to engage in battle? (Y/N): ";
            if(getValidChoice()) {
                Monster goblins(1000, 15);
                cout << "Rogue Goblins appeared!" << "\n";
                cout << "\n";
                Sleep(2500);
                cout << "The battle begins!" << "\n";
                cout << "\n";
                Sleep(2500);
                team.combat(goblins);
            } else {
                runAway++;
            }

            // 第三次小怪遭遇
            cout << "After emerging from the jungle." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "You prepare to set off for the dungeon." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "Along the way, you encounter an adventurer looking to explore treasure." << "\n";
            cout << "\n";
            Sleep(2500);
            if(mode == 1){
                cout << "He sees you as a chill guy, so he decides to become your good friend." << "\n";
                cout << "\n";
                Sleep(2500);
            }
            else{
                cout << "Who said they also wanted to defeat the boss." << "\n";
                cout << "\n";
                randomCharacter(team);
                Sleep(2500);
            }

            cout << "Inside a dungeon, a skeletal knight guards the treasure." << "\n";
            cout << "\n";
            Sleep(2500);

            cout << "Do you want to engage in battle? (Y/N): ";
            if(getValidChoice()) {
                Monster knight(2800, 24);
                cout << "Skeletal Knight appeared!" << "\n";
                cout << "\n";
                Sleep(2500);
                cout << "The battle begins!" << "\n";
                cout << "\n";
                Sleep(2500);
                team.combat(knight);
            } else {
                runAway++;
            }

            // 商店購買
            cout << "After surviving the dungeon, you arrive at a small village with a shop." << "\n";
             cout << "\n";
            Sleep(2500);
            store(team);

            // BOSS戰
            cout << "With your new equipment, you reach the pixelated castle and confront the Demon King." << "\n";
            cout << "\n";
            Sleep(2500);
            Monster boss(5000, 30);
            cout << "Final Boss: Demon King appeared!" << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "The battle begins!" << "\n";
            cout << "\n";
            Sleep(2500);
            team.combat(boss);
            cout << "You defeat the Demon King and rescue the princess." << "\n";
            cout << "\n";
            Sleep(2500);
            cout << "As the world starts to glitch, you wake up in front of an old arcade machine. :D" << "\n";
        }
        cout << "press 1 to play again or not 1 to close the game: " << "\n";
        int playAgain;
        cin >> playAgain;
        if (playAgain != 1) {
            break;
        }
    }
    return 0;
}
