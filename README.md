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

default_random_engine generator(time(0));
uniform_real_distribution<double> distribution(0.0, 1.0);

// Monster 类
class Monster {
private:
    int hp;
    int attack;

public:
    Monster(int h, int atk) : hp(h), attack(atk) {}

    int getHP() const { return hp; }
    void setHP(int newHP) { hp = max(0, newHP); }
    int getAttack() const { return attack; }

    // 嘗試暴擊
    bool tryCrit() const {
        static default_random_engine generator(static_cast<unsigned>(time(0)));
        static uniform_real_distribution<double> distribution(0.0, 1.0);
        return distribution(generator) < 0.25; // 25% 暴擊機率
    }

    void print() { cout << "Monster HP: " << max(0, hp) << ", Attack: " << attack << "\n"; }
};

enum Element { NONE, WATER, FIRE, WOOD }; // 元素類型

// Base Character class
class Character {
protected:
    string name;
    int level; // 角色等级
    int hp, maxHP; // Health Points
    int mp, maxMP; // Mana Points
    int armor, maxArmor;
    int attackPower; // 攻擊力
    int str, vit, agl, dex, intel; // 屬性值
    int skillCost, elementSkillCost; // 技能消耗 MP
    bool isDefending; // 是否处于防御状态
    Element element; // 角色元素属性
    unordered_map<string, int> statusEffects; // 状态效果存储

public:
    Character(string n, int lv, int s, int v, int ag, int d, int i) : name(n), level(lv), str(s), vit(v), agl(ag), dex(d), intel(i) {
        if (s + v + ag + d + i > 15 || s > 10 || v > 10 || ag > 10 || d > 10 || i > 10) {
            cout << "Invalid attribute points for character " << name << ".\n";
            exit(1);
        }

        hp = 20 + vit * 20;
        mp = 30 + intel * 10;
        armor = 10 + dex * 2 + agl * 1;
        attackPower = 10 + str * 3 + intel * 2;
        maxArmor = armor;
        maxHP = hp;
        maxMP = mp;
        element = NONE; // 默认没有元素
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
        cout << fixed << setprecision(1)  // 设置浮点数格式
            << name << " (Lv " << level << ") - HP: " << hp << "/" << maxHP << " (" << getHPPercentage() << "%), "
            << "MP: " << mp << "/" << maxMP << " (" << getMPPercentage() << "%), "
            << "Armor: " << armor << "/" << maxArmor << " (" << getArmorPercentage() << "%), "
            << "Attack: " << attackPower << "\n";
    }
    
    virtual void useSkill(Monster& monster) = 0;
    virtual void useElementSkill(Character& target) = 0; // 元素技能接口
    virtual void useElementSkill(Monster& target) {
        cout << "Elemental skill used on monster!\n";
        // 处理怪物相关的技能效果
    }

    bool isAlive() const { return hp > 0; }
    int getHP() const { return hp; }
    int getMaxHP() const { return maxHP; }
    void setHP(int newHP) {
        int damage = hp - newHP;

        if (damage > 0 && isDefending) {
            if (armor > 0) {
                int reducedDamage = damage / 2; // 防御减少一半伤害
                int armorCost = damage / 3;    // 防御消耗护甲值（假设 1/3 比例）
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
    void setMP(int newMP) { mp = newMP; }
    int getArmor() const { return armor; }
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
        Sleep(500); // 模拟短暂延迟，增强沉浸感
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

// Warrior 类
class Warrior : public Character {
private:
    int skillCost; // 技能消耗 MP
    int elementSkillCost; // 元素技能消耗 MP

    // 元素克制逻辑
    bool isEffectiveAgainst(Element targetElement) {
        static unordered_map<Element, Element> effectiveMap = {
            {WATER, FIRE},
            {FIRE, WOOD},
            {WOOD, WATER}
        };
        return effectiveMap[element] == targetElement;
    }

public:
    Warrior(string n, int lv, int s, int v, int ag, int d, int i): Character(n, lv, s, v, ag, d, i) {skillCost = 10; elementSkillCost = 20; element = FIRE;}

    void print() override {
        cout << "Warrior: ";
        Character::print();
    }

    void useSkill(Monster& monster) override {
        int damage = 40; // 普通技能固定傷害
        if (mp >= skillCost) { // 檢查 MP 是否足夠
            mp -= skillCost; // 消耗 MP
            monster.setHP(monster.getHP() - damage);
            cout << name << " used Power Strike! Dealt " << damage << " damage to the monster.\n";
            cout << "MP remaining: " << mp << "/" << maxMP << "\n";
        } else {
            cout << name << " does not have enough MP to use Power Strike!\n";
        }
    }

    void useElementSkill(Character& target) override {
        int effect = 20; // 基础效果
        if (mp >= elementSkillCost) {
            mp -= elementSkillCost;

            if (target.getName() == "Monster") {
                // 敌人逻辑
                if (isEffectiveAgainst(target.getElement())) {
                    effect *= 2; // 属性克制加倍
                    cout << name << "'s elemental skill is SUPER effective against " << target.getName() << "!\n";
                    target.applyStatusEffect("Vulnerable", 1); // 施加 Vulnerable 状态
                    cout << target.getName() << " is now vulnerable and takes double damage for 1 turn.\n";
                }

                target.setHP(target.getHP() - effect);
                cout << name << " used Elemental Skill on " << target.getName()
                    << "! Dealt " << effect << " damage.\n";
            } else {
                // 队友逻辑（恢复 HP）
                if (target.getElement() == element) {
                    effect *= 2; // 属性相同恢复加倍
                    cout << name << "'s elemental skill synergizes with " << target.getName() << "!\n";
                }

                target.setHP(target.getHP() + effect);
                target.applyStatusEffect("Empowered", 1); // 施加 Empowered 状态
                cout << target.getName() << " is now empowered and deals double damage for 1 turn.\n";
            }
            cout << "MP remaining: " << mp << "/" << maxMP << "\n";

        } else {
            cout << name << " does not have enough MP to use Elemental Skill!\n";
        }
    }
};

// Wizard 类
class Wizard : public Character {
private:
    int skillCost; // 技能消耗 MP
    int elementSkillCost; // 元素技能消耗 MP

    // 元素克制逻辑
    bool isEffectiveAgainst(Element targetElement) {
        static unordered_map<Element, Element> effectiveMap = {
            {WATER, FIRE},
            {FIRE, WOOD},
            {WOOD, WATER}
        };
        return effectiveMap[element] == targetElement;
    }
public:
    Wizard(string n, int lv, int s, int v, int ag, int d, int i) : Character(n, lv, s, v, ag, d, i) {skillCost = 15; elementSkillCost = 30; element = WATER;}

    void print() override {
        cout << "Wizard: ";
        Character::print();
    }

    void useSkill(Monster& monster) override {
        int damage = 30;  // 魔法飛彈固定傷害
        if (mp >= skillCost) {  // 檢查 MP 是否足夠
            mp -= skillCost;  // 消耗 MP
            monster.setHP(monster.getHP() - damage);
            cout << name << " used Magic Missile! Dealt " << damage << " damage to the monster.\n";
            cout << "MP remaining: " << mp << "/" << maxMP << "\n";
        } else {
            cout << name << " does not have enough MP to use Magic Missile!\n";
        }
    }

    void useElementSkill(Character& target) override {
        int effect = 20; // 基础效果
        if (mp >= elementSkillCost) {
            mp -= elementSkillCost;

            if (target.getName() == "Monster") {
                // 敌人逻辑
                if (isEffectiveAgainst(target.getElement())) {
                    effect *= 2; // 属性克制加倍
                    cout << name << "'s elemental skill is SUPER effective against " << target.getName() << "!\n";
                    target.applyStatusEffect("Vulnerable", 1); // 施加 Vulnerable 状态
                    cout << target.getName() << " is now vulnerable and takes double damage for 1 turn.\n";
                }

                target.setHP(target.getHP() - effect);
                cout << name << " used Elemental Skill on " << target.getName()
                    << "! Dealt " << effect << " damage.\n";
            } else {
                // 队友逻辑（恢复 HP）
                if (target.getElement() == element) {
                    effect *= 2; // 属性相同恢复加倍
                    cout << name << "'s elemental skill synergizes with " << target.getName() << "!\n";
                }

                target.setHP(target.getHP() + effect);
                target.applyStatusEffect("Empowered", 1); // 施加 Empowered 状态
                cout << target.getName() << " is now empowered and deals double damage for 1 turn.\n";
            }
            cout << "MP remaining: " << mp << "/" << maxMP << "\n";

        } else {
            cout << name << " does not have enough MP to use Elemental Skill!\n";
        }
    }
};

// Tank 类
class Tank : public Character {
private:
    int skillCost; // 技能消耗 MP
    int elementSkillCost; // 元素技能消耗 MP

    // 元素克制逻辑
    bool isEffectiveAgainst(Element targetElement) {
        static unordered_map<Element, Element> effectiveMap = {
            {WATER, FIRE},
            {FIRE, WOOD},
            {WOOD, WATER}
        };
        return effectiveMap[element] == targetElement;
    }
public:
    Tank(string n, int lv, int s, int v, int ag, int d, int i) : Character(n, lv, s, v, ag, d, i) {skillCost = 5; elementSkillCost = 10; element = WOOD;}

    void print() override {
        cout << "Tank: ";
        Character::print();
    }

    void useSkill(Monster& monster) override {
        int damage = 20;  // 盾牌猛擊固定傷害
        if (mp >= skillCost) {  // 檢查 MP 是否足夠
            mp -= skillCost;  // 消耗 MP
            monster.setHP(monster.getHP() - damage);
            cout << name << " used Shield Bash! Dealt " << damage << " damage to the monster.\n";
            cout << "MP remaining: " << mp << "/" << maxMP << "\n";
        } else {
            cout << name << " does not have enough MP to use Magic Missile!\n";
        }
    }

    void useElementSkill(Character& target) override {
        int effect = 20; // 基础效果
        if (mp >= elementSkillCost) {
            mp -= elementSkillCost;

            if (target.getName() == "Monster") {
                // 敌人逻辑
                if (isEffectiveAgainst(target.getElement())) {
                    effect *= 2; // 属性克制加倍
                    cout << name << "'s elemental skill is SUPER effective against " << target.getName() << "!\n";
                    target.applyStatusEffect("Vulnerable", 1); // 施加 Vulnerable 状态
                    cout << target.getName() << " is now vulnerable and takes double damage for 1 turn.\n";
                }

                target.setHP(target.getHP() - effect);
                cout << name << " used Elemental Skill on " << target.getName()
                    << "! Dealt " << effect << " damage.\n";
            } else {
                // 队友逻辑（恢复 HP）
                if (target.getElement() == element) {
                    effect *= 2; // 属性相同恢复加倍
                    cout << name << "'s elemental skill synergizes with " << target.getName() << "!\n";
                }

                target.setHP(target.getHP() + effect);
                target.applyStatusEffect("Empowered", 1); // 施加 Empowered 状态
                cout << target.getName() << " is now empowered and deals double damage for 1 turn.\n";
            }
            cout << "MP remaining: " << mp << "/" << maxMP << "\n";

        } else {
            cout << name << " does not have enough MP to use Elemental Skill!\n";
        }
    }
};

// Elf 类
class Elf : public Character {
private:
    int skillCost; // 技能消耗 MP
    int elementSkillCost; // 元素技能消耗 MP

    // 元素克制逻辑
    bool isEffectiveAgainst(Element targetElement) {
        static unordered_map<Element, Element> effectiveMap = {
            {WATER, FIRE},
            {FIRE, WOOD},
            {WOOD, WATER}
        };
        return effectiveMap[element] == targetElement;
    }
public:
    Elf(string n, int lv, int s, int v, int ag, int d, int i) : Character(n, lv, s, v, ag, d, i) {skillCost = 15; elementSkillCost = 30; element = WOOD;}

    void print() override {
        cout << "Elf: ";
        Character::print();
    }

    void useSkill(Monster& monster) override {
        int damage = 30;  // 箭雨固定伤害
        if (mp >= skillCost) {  // 檢查 MP 是否足夠
            mp -= skillCost;  // 消耗 MP
            monster.setHP(monster.getHP() - damage);
            cout << name << " used Rain of Arrows! Dealt " << damage << " damage to the monster.\n";
            cout << "MP remaining: " << mp << "/" << maxMP << "\n";
        } else {
            cout << name << " does not have enough MP to use Magic Missile!\n";
        }
    }

    void useElementSkill(Character& target) override {
        int effect = 20; // 基础效果
        if (mp >= elementSkillCost) {
            mp -= elementSkillCost;

            if (target.getName() == "Monster") {
                // 敌人逻辑
                if (isEffectiveAgainst(target.getElement())) {
                    effect *= 2; // 属性克制加倍
                    cout << name << "'s elemental skill is SUPER effective against " << target.getName() << "!\n";
                    target.applyStatusEffect("Vulnerable", 1); // 施加 Vulnerable 状态
                    cout << target.getName() << " is now vulnerable and takes double damage for 1 turn.\n";
                }

                target.setHP(target.getHP() - effect);
                cout << name << " used Elemental Skill on " << target.getName()
                    << "! Dealt " << effect << " damage.\n";
            } else {
                // 队友逻辑（恢复 HP）
                if (target.getElement() == element) {
                    effect *= 2; // 属性相同恢复加倍
                    cout << name << "'s elemental skill synergizes with " << target.getName() << "!\n";
                }

                target.setHP(target.getHP() + effect);
                target.applyStatusEffect("Empowered", 1); // 施加 Empowered 状态
                cout << target.getName() << " is now empowered and deals double damage for 1 turn.\n";
            }
            cout << "MP remaining: " << mp << "/" << maxMP << "\n";

        } else {
            cout << name << " does not have enough MP to use Elemental Skill!\n";
        }
    }
};

// Healer 类
class Healer : public Character {
private:
    int skillCost; // 技能消耗 MP
    int elementSkillCost; // 元素技能消耗 MP

    // 元素克制逻辑
    bool isEffectiveAgainst(Element targetElement) {
        static unordered_map<Element, Element> effectiveMap = {
            {WATER, FIRE},
            {FIRE, WOOD},
            {WOOD, WATER}
        };
        return effectiveMap[element] == targetElement;
    }
public:
    Healer(string n, int lv, int s, int v, int ag, int d, int i) : Character(n, lv, s, v, ag, d, i) {skillCost = 20; elementSkillCost = 40; element = WATER;}

    void print() override {
        cout << "Healer: ";
        Character::print();
    }

    void useSkill(Monster& monster) override {
        int healing = 20;  // 治疗光环固定治疗量
        if (mp >= skillCost) {  // 檢查 MP 是否足夠
            mp -= skillCost;  // 消耗 MP
            hp += healing;
            cout << name << " used Healing Aura! Restored " << healing << " HP.\n";
            cout << "MP remaining: " << mp << "/" << maxMP << "\n";
        } else {
            cout << name << " does not have enough MP to use Magic Missile!\n";
        }
    }

    void useElementSkill(Character& target) override {
        int effect = 20; // 基础效果
        if (mp >= elementSkillCost) {
            mp -= elementSkillCost;

            if (target.getName() == "Monster") {
                // 敌人逻辑
                if (isEffectiveAgainst(target.getElement())) {
                    effect *= 2; // 属性克制加倍
                    cout << name << "'s elemental skill is SUPER effective against " << target.getName() << "!\n";
                    target.applyStatusEffect("Vulnerable", 1); // 施加 Vulnerable 状态
                    cout << target.getName() << " is now vulnerable and takes double damage for 1 turn.\n";
                }

                target.setHP(target.getHP() - effect);
                cout << name << " used Elemental Skill on " << target.getName()
                    << "! Dealt " << effect << " damage.\n";
            } else {
                // 队友逻辑（恢复 HP）
                if (target.getElement() == element) {
                    effect *= 2; // 属性相同恢复加倍
                    cout << name << "'s elemental skill synergizes with " << target.getName() << "!\n";
                }

                target.setHP(target.getHP() + effect);
                target.applyStatusEffect("Empowered", 1); // 施加 Empowered 状态
                cout << target.getName() << " is now empowered and deals double damage for 1 turn.\n";
            }
            cout << "MP remaining: " << mp << "/" << maxMP << "\n";

        } else {
            cout << name << " does not have enough MP to use Elemental Skill!\n";
        }
    }
};

// Goblin 类
class Goblin : public Character {
private:
    int skillCost; // 技能消耗 MP
    int elementSkillCost; // 元素技能消耗 MP

    // 元素克制逻辑
    bool isEffectiveAgainst(Element targetElement) {
        static unordered_map<Element, Element> effectiveMap = {
            {WATER, FIRE},
            {FIRE, WOOD},
            {WOOD, WATER}
        };
        return effectiveMap[element] == targetElement;
    }
public:
    Goblin(string n, int lv, int s, int v, int ag, int d, int i) : Character(n, lv, s, v, ag, d, i) {skillCost = 10; elementSkillCost = 20; element = WOOD;}

    void print() override {
        cout << "Goblin: ";
        Character::print();
    }

    void useSkill(Monster& monster) override {
        int damage = 20;  // 突袭固定伤害
        if (mp >= skillCost) {  // 檢查 MP 是否足夠
            mp -= skillCost;  // 消耗 MP
            monster.setHP(monster.getHP() - damage);
            cout << name << " used Ambush! Dealt " << damage << " damage to the monster.\n";
            cout << "MP remaining: " << mp << "/" << maxMP << "\n";
        } else {
            cout << name << " does not have enough MP to use Magic Missile!\n";
        }
    }

    void useElementSkill(Character& target) override {
        int effect = 20; // 基础效果
        if (mp >= elementSkillCost) {
            mp -= elementSkillCost;

            if (target.getName() == "Monster") {
                // 敌人逻辑
                if (isEffectiveAgainst(target.getElement())) {
                    effect *= 2; // 属性克制加倍
                    cout << name << "'s elemental skill is SUPER effective against " << target.getName() << "!\n";
                    target.applyStatusEffect("Vulnerable", 1); // 施加 Vulnerable 状态
                    cout << target.getName() << " is now vulnerable and takes double damage for 1 turn.\n";
                }

                target.setHP(target.getHP() - effect);
                cout << name << " used Elemental Skill on " << target.getName()
                    << "! Dealt " << effect << " damage.\n";
            } else {
                // 队友逻辑（恢复 HP）
                if (target.getElement() == element) {
                    effect *= 2; // 属性相同恢复加倍
                    cout << name << "'s elemental skill synergizes with " << target.getName() << "!\n";
                }

                target.setHP(target.getHP() + effect);
                target.applyStatusEffect("Empowered", 1); // 施加 Empowered 状态
                cout << target.getName() << " is now empowered and deals double damage for 1 turn.\n";
            }
            cout << "MP remaining: " << mp << "/" << maxMP << "\n";

        } else {
            cout << name << " does not have enough MP to use Elemental Skill!\n";
        }
    }
};

// Knight 类
class Knight : public Character {
private:
    int skillCost; // 技能消耗 MP
    int elementSkillCost; // 元素技能消耗 MP

    // 元素克制逻辑
    bool isEffectiveAgainst(Element targetElement) {
        static unordered_map<Element, Element> effectiveMap = {
            {WATER, FIRE},
            {FIRE, WOOD},
            {WOOD, WATER}
        };
        return effectiveMap[element] == targetElement;
    }
public:
    Knight(string n, int lv, int s, int v, int ag, int d, int i) : Character(n, lv, s, v, ag, d, i) {skillCost = 15; elementSkillCost = 30; element = FIRE;}

    void print() override {
        cout << "Knight: ";
        Character::print();
    }

    void useSkill(Monster& monster) override {
        int damage = 30;  // 狂暴攻击固定伤害
        if (mp >= skillCost) {  // 檢查 MP 是否足夠
            mp -= skillCost;  // 消耗 MP
            monster.setHP(monster.getHP() - damage);
            cout << name << " used Frenzy! Dealt " << damage << " damage to the monster.\n";
            cout << "MP remaining: " << mp << "/" << maxMP << "\n";
        } else {
            cout << name << " does not have enough MP to use Magic Missile!\n";
        }
    }

    void useElementSkill(Character& target) override {
        int effect = 20;  // 元素技能基礎加成或傷害
        if (mp >= elementSkillCost) {  // 檢查 MP 是否足夠
            mp -= elementSkillCost;  // 消耗 MP
            if (isEffectiveAgainst(target.getElement())) {
                effect *= 2;  // 若克制，效果加倍
                cout << name << "'s elemental skill is SUPER effective!\n";
            }

            // 判斷是隊友還是敵人
            if (target.getHP() > 0) {
                target.setHP(target.getHP() + effect);  // 增加隊友的 HP
                cout << name << " used Elemental Blessing on " << target.getName()
                     << "! Restored " << effect << " HP.\n";
            } else {
                cout << "Invalid target for elemental skill.\n";
            }
            cout << "MP remaining: " << mp << "/" << maxMP << "\n";
        } else {
            cout << name << " does not have enough MP to use Elemental Blessing!\n";
        }
    }
};

// Team 类
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

    void print() const {
        for (auto member : members) {
            member->print();
        }
    }

    void combat(Monster& monster) {
        while (true) {
            // 显示怪物状态
            cout << "\nMonster's Status:\n";
            monster.print();

            // 玩家回合
            for (size_t turn = 0; turn < members.size(); turn++) {
                cout << "\nSelect a team member to take action:\n";
                for (size_t i = 0; i < members.size(); i++) {
                    cout << i + 1 << ". ";
                    members[i]->print(); // 显示角色的状态
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

                // 选择行动
                cout << "1. Normal Attack\n2. Use Skill\n3. Use Element Skill\n4. Skip Turn\nChoose an action: ";
                int action;
                cin >> action;

                switch (action) {
                    case 1: {
                        // 普通攻击
                        bool crit = member->tryCrit();
                        int damage = crit ? member->getAttack() * 2 : member->getAttack();
                        monster.setHP(monster.getHP() - damage);
                        cout << member->getName() << " attacked the monster, dealing " << damage << (crit ? " (Critical Hit!)" : "") << " damage!\n";
                        break;
                    }
                    case 2: {
                        // 使用技能
                        if (member->getMP() >= member->getSkillCost()) {
                            member->useSkill(monster);
                            member->setMP(member->getMP() - member->getSkillCost());
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
                            // 对怪物使用元素技能
                            member->useElementSkill(monster);
                        } else if (targetAction == 2) {
                            // 对队友使用元素技能
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

                // 如果怪物被击败，结束战斗
                if (monster.getHP() <= 0) {
                    cout << "\nThe team has defeated the monster!\n";
                    return;
                }
            }

            // 怪物回合
            cout << "\nMonster's turn!\n";
            for (auto& member : members) {
                if (member->isAlive()) {
                    // 怪物攻击
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

            // 如果全队成员都死了，结束战斗
            if (all_of(members.begin(), members.end(), [](Character* c) { return !c->isAlive(); })) {
                cout << "\nThe team has been defeated...\n";
                return;
            }
        }
    }
};

// 主程序
int main() {
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
    cin >> mHP >> mAttack;  // 输入怪物的血量与攻击力
    Monster monster(mHP, mAttack);

    Team team;
    string name;
    int level, role, s, v, ag, d, i, hp, armor, attackPower;

    cout << "Enter character details (name, role, level, str, vit, agl, dex, intel) or -1 to stop:" << "\n";

    while (true) {
        cin >> name;
        if (name == "-1") break;
        cin >> role >> level >> s >> v >> ag >> d >> i;

        // 创建角色并确保属性分配符合条件
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
