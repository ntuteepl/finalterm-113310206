[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/_v8RbUGg)
#include<iostream>
#include<vector>
#include<algorithm>
#include<random>
#include<ctime>
#include<string>
#include<cmath>
#include <iomanip>
#include <unordered_map>
#include <windows.h>
using namespace std;

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

    void applyStatusEffect(string effect, int duration) { statusEffects[effect] = duration; }
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
    int skillCost, elementSkillCost; // 技能消耗 MP
    bool isDefending; // 是否處於防禦狀態
    Element element; // 角色元素屬性
    unordered_map<string, int> statusEffects; // 狀態效果存儲

public:
    Character(string n, int lv, int s, int v, int ag, int d, int i) : name(n), level(lv), str(s), vit(v), agl(ag), dex(d), intel(i) {
        if (s + v + ag + d + i > 15 || s > 10 || v > 10 || ag > 10 || d > 10 || i > 10) {
            cout << "Invalid attribute points for character " << name << ".\n";
            exit(1);
        }

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
        cout << fixed << setprecision(1)  // 設置浮點數格式
            << name << " (Lv " << level << ") - HP: " << hp << "/" << maxHP << " (" << getHPPercentage() << "%), "
            << "MP: " << mp << "/" << maxMP << " (" << getMPPercentage() << "%), "
            << "Armor: " << armor << "/" << maxArmor << " (" << getArmorPercentage() << "%), "
            << "Attack: " << attackPower << "\n";
    }
    
    virtual void useSkill(Monster& monster) = 0;
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
                int reducedDamage = damage / 2; // 防禦減少一半傷害
                int armorCost = damage * 0.2;    // 防禦消耗護甲值（假設 1/3 比例）
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
    int getSkillCost() const { return skillCost; }
    int getElementSkillCost() const { return elementSkillCost; }
    string getName() const { return name; }
    Element getElement() const { return element; }
    void applyStatusEffect(string effect, int duration) { statusEffects[effect] = duration; }
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
            cout << "MP remaining: " << user.getMP() << "/" << user.getMaxMP() << "\n";
        } else {
            cout << user.getName() << " does not have enough MP to use " << name << "!\n";
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
            target.setElement(element); // 附加屬性效果
            cout << user.getName() << " used " << name << "! The monster is now affected by " << user.getElementName() << " element for 2 turns.\n";
            cout << "MP remaining: " << user.getMP() << "/" << user.getMaxMP() << "\n";
        } else {
            cout << user.getName() << " does not have enough MP to use " << name << "!\n";
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
            }

            target.setHP(target.getHP() + finalEffect);
            cout << user.getName() << " used " << name << " on " << target.getName() << "! Restored " << finalEffect << " HP.\n";
            cout << "MP remaining: " << user.getMP() << "/" << user.getMaxMP() << "\n";
        } else {
            cout << user.getName() << " does not have enough MP to use " << name << "!\n";
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
        : Character(n, lv, s, v, ag, d, i), skill("Power Strike", 10, 40), elementSkill("Flame Slash", 20, 50, FIRE) {
        element = FIRE;
    }

    void print() override {
        cout << "Warrior: ";
        Character::print();
    }

    void useSkill(Monster& monster) override {
        int damage = getAttack() * 1.5;
        monster.setHP(monster.getHP() - damage);
        cout << getName() << " used Power Strike! Dealt " << damage << " damage to the monster.\n";
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
        : Character(n, lv, s, v, ag, d, i), skill("Arcane Shield", 15, 0), elementSkill("Water Blast", 30, 60, WATER) {
        element = WATER;
    }

    void print() override {
        cout << "Wizard: ";
        Character::print();
    }

    void useSkill(Monster& monster) override {
        applyStatusEffect("Invincible", 1);
        cout << getName() << " used Arcane Shield! Will ignore the next attack.\n";
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
        : Character(n, lv, s, v, ag, d, i), skill("Fortify", 5, 0), elementSkill("Wooden Guard", 10, 30, WOOD) {
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
        : Character(n, lv, s, v, ag, d, i), skill("Arrow Barrage", 15, 20), elementSkill("Nature's Wrath", 30, 60, WOOD) {
        element = WOOD;
    }

    void print() override {
        cout << "Elf: ";
        Character::print();
    }

    void useSkill(Monster& monster) override {
        int damage = getAttack() * 1.1;
        monster.setHP(monster.getHP() - damage);
        cout << getName() << " used Arrow Barrage! Dealt " << damage << " damage to the monster.\n";
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
        : Character(n, lv, s, v, ag, d, i), skill("Healing Light", 20, 25), elementSkill("Holy Water", 40, 50, WATER) {
        element = WATER;
    }

    void print() override {
        cout << "Healer: ";
        Character::print();
    }

    void useSkill(Monster& monster) override {
        setHP(getHP() + 25);
        cout << getName() << " used Healing Light! Restored 25 HP.\n";
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
        : Character(n, lv, s, v, ag, d, i), skill("Sneak Attack", 10, 30), elementSkill("Poison Dart", 20, 40, WOOD) {
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
        : Character(n, lv, s, v, ag, d, i), skill("Shield Charge", 15, 25), elementSkill("Fire Lance", 30, 60, FIRE) {
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

public:
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

    void combat(Monster& monster) {
        while (true) {
            // 顯示怪物狀態
            cout << "\nMonster's Status:\n";
            monster.print();

            // 玩家回合
            for (size_t turn = 0; turn < members.size(); turn++) {
                cout << "\nSelect a team member to take action:\n";
                for (size_t i = 0; i < members.size(); i++) {
                    cout << i + 1 << ". ";
                    members[i]->print(); // 顯示角色的狀態
                }

                int choice;
                cout << "Choose a team member (1-" << members.size() << "): ";
                cin >> choice;

                if (choice < 1 || choice > members.size() || !members[choice - 1]->isAlive()) {
                    cout << "Invalid choice or character is not alive. Turn skipped.\n";
                    continue;
                }

                Character* member = members[choice - 1];
                cout << "\nIt's " << member->getName() << "'s turn.\n";

                // 選擇行動
                cout << "1. Normal Attack\n2. Use Skill\n3. Use Element Skill\n4. Skip Turn\nChoose an action: ";
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
                        monster.setHP(monster.getHP() - damage);
                        cout << member->getName() << " attacked the monster, dealing " << damage << (crit ? " (Critical Hit!)" : "") << " damage!\n";
                        break;
                    }
                    case 2: {
                        // 使用技能
                        if (member->getMP() >= member->getSkillCost()) {
                            member->useSkill(monster);
                        } else {
                            cout << "Not enough MP to use skill.\n";
                        }
                        break;
                    }
                    case 3: {
                        // 使用元素技能
                        cout << "1. Use Element Skill on Monster\n2. Use Element Skill on a Teammate\nChoose a target: ";
                        int targetAction;
                        cin >> targetAction;

                        if (targetAction == 1) {
                            // 對怪物使用元素技能
                            member->useElementSkill(monster);
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
                    return;
                }
            }

            // 怪物回合
            cout << "\nMonster's turn!\n";
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
                    }
                }
            }

            // 如果全隊成員都死了，結束戰鬥
            if (all_of(members.begin(), members.end(), [](Character* c) { return !c->isAlive(); })) {
                cout << "\nThe team has been defeated...\n";
                return;
            }
        }
    }
};

// 主程序
int main() {
    Team team; // 創建 Team 實例
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
    Sleep(1000);
    int mHP, mAttack;
    cout << "Enter the monster's HP and attack power: ";
    cin >> mHP >> mAttack;  // 輸入怪物的血量與攻擊力
    Monster monster(mHP, mAttack);

    string name;
    int level, role, s, v, ag, d, i, hp, armor, attackPower;

    cout << "Enter character details (name, role, level, str, vit, agl, dex, intel) or -1 to stop:" << "\n";

    while (true) {
        cin >> name;
        if (name == "-1") break;
        cin >> role >> level >> s >> v >> ag >> d >> i;

        // 創建角色並確保屬性分配符合條件
        Character* newCharacter = nullptr;
        if (role == 1) newCharacter = new Warrior(name, level, s, v, ag, d, i);
        else if (role == 2) newCharacter = new Wizard(name, level, s, v, ag, d, i);
        else if (role == 3) newCharacter = new Tank(name, level, s, v, ag, d, i);
        else if (role == 4) newCharacter = new Elf(name, level, s, v, ag, d, i);
        else if (role == 5) newCharacter = new Healer(name, level, s, v, ag, d, i);
        else if (role == 6) newCharacter = new Goblin(name, level, s, v, ag, d, i);
        else if (role == 7) newCharacter = new Knight(name, level, s, v, ag, d, i);

        if (newCharacter) {
            team.addCharacter(newCharacter);
        }
    }

    team.combat(monster);
    return 0;
}
