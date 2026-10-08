#include "water_seven.h"

////////////////////////////////////////////////////////////////////////
/// STUDENT'S ANSWER BEGINS HERE
/// Complete the following functions
/// DO NOT modify any parameters in the functions.
////////////////////////////////////////////////////////////////////////
//Task 0
// Hàm phụ để giới hạn giá trị trong khoảng [low, high]
int clamp(int val, int low, int high) {
    if (val < low) return low;
    if (val > high) return high;
    return val;
}
bool readInput(
    const string &filename,
    char character[FIXED_CHARACTER][MAX_NAME],
    int hp[FIXED_CHARACTER],
    int skill[FIXED_CHARACTER],
    int &shipHP,
    int &repairCost)
{
    ifstream file(filename);
    if (!file.is_open()) return false;

    string name;
    int val1, val2;
    int currentChars = 0;

    const string crew[] = {"LUFFY", "ZORO", "SANJI", "NAMI", "CHOPPER", "USOPP", "ROBIN"};

    while (file >> name >> val1 >> val2) {
        if (name == "GOING_MERRY") {
            shipHP = clamp(val1, 0, 1000);
            repairCost = clamp(val2, 0, 3000);
        } 
        else {
            bool isValidCrew = false;
            for (int i = 0; i < 7; i++) {
                if (name == crew[i]) {
                    isValidCrew = true;
                    break;
                }
            }

            if (isValidCrew) {
                int clampedHP = clamp(val1, 0, 1000);
                int clampedSkill = clamp(val2, 0, 100);
                int foundIdx = -1;
                for (int i = 0; i < currentChars; i++) {
                    if (strcmp(character[i], name.c_str()) == 0) {
                        foundIdx = i;
                        break;
                    }
                }

                if (foundIdx != -1) {
                    hp[foundIdx] = clampedHP;
                    skill[foundIdx] = clampedSkill;
                } else if (currentChars < FIXED_CHARACTER) {
                    strcpy(character[currentChars], name.c_str());
                    hp[currentChars] = clampedHP;
                    skill[currentChars] = clampedSkill;
                    currentChars++;
                }
            }
        }
    }
    file.close();
    return true;
}

// Task 1
int damageEvaluation(int shipHP, int repairCost) {
    int tempHP = shipHP;
    int sum_of_digits = 0;

    if (tempHP == 0) sum_of_digits = 0;
    else {
        while (tempHP > 0) {
            sum_of_digits += tempHP % 10;
            tempHP /= 10;
        }
    }

    int sum_of_divisors = 0;
    for (int i = 1; i < sum_of_digits; i++) {
        if (sum_of_digits % i == 0) {
            sum_of_divisors += i;
        }
    }

    bool isPerfect = (sum_of_digits > 0 && sum_of_divisors == sum_of_digits);
    int finalCost = repairCost;

    if (shipHP < 455 && isPerfect) {
        finalCost = (int)ceil(1.5 * repairCost);
    }

    if (finalCost > 3000) finalCost = 3000;
    if (finalCost < 0) finalCost = 0;

    return finalCost;
}

// Task 2
int conflictSimulation(
    char character[FIXED_CHARACTER][MAX_NAME], int hp[FIXED_CHARACTER], int skill[FIXED_CHARACTER],
    int shipHP, int repairCost){
        //TODO
        int luffyidx = -1, usoppidx = -1;        
        for (int i = 0; i < FIXED_CHARACTER; i++) {
            if (strcmp(character[i], "LUFFY") == 0) luffyidx = i;
            else if (strcmp(character[i], "USOPP") == 0) usoppidx = i;
        }
        if (luffyidx == -1 || usoppidx == -1) return -1;
        int adjustment = (int)ceil((double)repairCost / 100.0 + (double)(500 - shipHP) / 50.0);
        int conflictIndex = skill[luffyidx] - skill[usoppidx] + adjustment;
        int addedconflict[6] = {255, 20, 50, 70, 90, 100};
        int eventCount = 0;
        while (conflictIndex < 255 && eventCount < 10) {
            int id = conflictIndex % 6;
            if (id < 0) id += 6; 
            conflictIndex += addedconflict[id];
            eventCount++;
        }
        return conflictIndex;
    }

// Task 3
void resolveDuel(
    char character[FIXED_CHARACTER][MAX_NAME], int hp[FIXED_CHARACTER], int skill[FIXED_CHARACTER],
    int conflictIndex, int repairCost, char duel[FIXED_CHARACTER][MAX_NAME]){
        //TODO: Output assign to duel parameter
    int luffyIdx = -1, usoppIdx = -1;
    int memberIndices[5];
    int memberCount = 0;
    for (int i = 0; i < FIXED_CHARACTER; i++) {
        if (character[i][0] != '\0') {
            if (strcmp(character[i], "LUFFY") == 0) luffyIdx = i;
            else if (strcmp(character[i], "USOPP") == 0) usoppIdx = i;
            else {
                memberIndices[memberCount++] = i;
            }
        }
    }
    if (luffyIdx == -1 || usoppIdx == -1) return;
    int U = skill[usoppIdx] + (int)ceil((double)conflictIndex / 20.0 + (double)repairCost / 500.0);
    if (skill[luffyIdx] >= U) return;
    for (int i = 0; i < FIXED_CHARACTER; i++) duel[i][0] = '\0';
    //Bitmask
    int bestMask = -1;  
    int minCost = INT_MAX;
    int minSize = INT_MAX;
    //2^5 cases
    for (int mask = 1; mask < (1 << memberCount); mask++) {
        int currentSupport = 0;
        int currentCost = 0;
        int currentSize = 0;
        for (int i = 0; i < memberCount; i++) {
            if ((mask >> i) & 1) {
                int idx = memberIndices[i];
                currentSupport += skill[idx];
                currentCost += (hp[idx] % 10) + 1;
                currentSize++;
            }
        }
        //find best cases' code
        if (skill[luffyIdx] + currentSupport >= U) {
            if (currentCost < minCost) {
                minCost = currentCost;
                minSize = currentSize;
                bestMask = mask;
            } else if (currentCost == minCost && currentSize < minSize) {
                minSize = currentSize;
                bestMask = mask;
            }
        }
    }
    //decode
    if (bestMask != -1) {
        int count = 0;
        for (int i = 0; i < memberCount; i++) {
            if ((bestMask >> i) & 1) {
                strcpy(duel[count++], character[memberIndices[i]]);
            }
        }
    }
}

// Task 4
void decodeCP9Message(char character[FIXED_CHARACTER][MAX_NAME], 
    int hp[FIXED_CHARACTER], int skill[FIXED_CHARACTER], int conflictIndex, 
    int repairCost, char cipherText[], char resultText[]){
        //TODO: Output assign to resultText parameter
    resultText[0] = '\0';
    string s(cipherText); 
    size_t hashPos = s.rfind('#');
    if (hashPos == string::npos) return;
    string message = s.substr(0, hashPos); 
    string checkStr = s.substr(hashPos + 1);
    if (checkStr.empty()) return;
    int providedChecksum = stoi(checkStr); 
    int calculatedChecksum = 0;
    for (int i = 0; i < message.length(); i++) {
        calculatedChecksum += (unsigned char)message[i];
    }
    if (calculatedChecksum % 100 != providedChecksum) return;
    int key = (conflictIndex + repairCost) % 26;
    int B = (key % 5) + 4;
    int n = message.length();
    for (int i = 0; i < n; i += B) {
        int left = i;
        int right = (i + B - 1 < n) ? (i + B - 1) : (n - 1);
        while (left < right) {
            char temp = message[left];
            message[left] = message[right];
            message[right] = temp;
            left++;
            right--;
        }
    }
    for (int i = 0; i < n; i++) {
        if (message[i] >= 'A' && message[i] <= 'Z') {
            message[i] = (message[i] - 'A' - (key % 26) + 26) % 26 + 'A';
        } else if (message[i] >= 'a' && message[i] <= 'z') {
            message[i] = (message[i] - 'a' - (key % 26) + 26) % 26 + 'a';
        } else if (message[i] >= '0' && message[i] <= '9') {
            message[i] = (message[i] - '0' - (key % 10) + 10) % 10 + '0';
        }
    }
    bool isTaskTrue = (message.find("CP9") != string::npos) || (message.find("ENIESLOBBY") != string::npos);
    if (isTaskTrue) {
        message += "_TRUE";
    } else {
        message += "_FALSE";
    }
    strcpy(resultText, message.c_str());
}

// Task 5
int analyzeDangerLimit(int grid[MAX_GRID][MAX_GRID], int rows, int cols){
    // TODO CODE
    int maxRowSum = 0;
    int maxCell = -1e9;
    for (int i = 0; i < rows; i++) {
        int currentRowSum = 0;
        for (int j = 0; j < cols; j++) {
            if (grid[i][j] > maxCell) maxCell = grid[i][j];
            if (grid[i][j] != -1) currentRowSum += grid[i][j];
        }
        if (currentRowSum > maxRowSum) maxRowSum = currentRowSum;
    }
    return maxRowSum + maxCell;
}
bool evaluateRoute(int grid[MAX_GRID][MAX_GRID], int rows, int cols, int dangerLimit){
    // TODO CODE
    int dp[MAX_GRID];
    if (grid[0][0] != -1) dp[0] = grid[0][0];
    else return false;
    for (int j = 1; j < cols; j++) {
        if (grid[0][j] != -1 && dp[j - 1] != -1) dp[j] = dp[j - 1] + grid[0][j];
        else dp[j] = -1;
    }
    for (int i = 1; i < rows; i++) {
        if (grid[i][0] == -1 || dp[0] == -1) dp[0] = -1;
        else dp[0] += grid[i][0];
        for (int j = 1; j < cols; j++) {
            if ((grid[i][j] == -1) || (dp[j - 1] == -1 && dp[j] == -1)) dp[j] = -1;
            else if (dp[j - 1] == -1) dp[j] += grid[i][j];
            else if (dp[j] == -1) dp[j] = dp[j - 1] + grid[i][j];
            else dp[j] = min(dp[j], dp[j - 1]) + grid[i][j];
        }
    }
    if (dp[cols - 1] == -1 || dp[cols - 1] > dangerLimit) return false;
    return true;
}

////////////////////////////////////////////////
/// END OF STUDENT'S ANSWER
////////////////////////////////////////////////