#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

constexpr int MAX_THRUST = 100;
constexpr int BOOST_ANGLE_THRESHOLD = 15;
const string boostExecuteKeyword = "BOOST";

struct Vec2 {
    int x;
    int y;

    Vec2(int _x = 0, int _y = 0) : x(_x), y(_y) {}

    bool operator==(const Vec2& other) const {
        return x == other.x && y == other.y;
    }
    Vec2 operator+(const Vec2& other) const {
        return Vec2(x + other.x, y + other.y);
    }

    Vec2 operator-(const Vec2& other) const {
        return Vec2(x - other.x, y - other.y);
    }

    Vec2 operator*(float scalar) const {
        return Vec2(x * scalar, y * scalar);
    }

    float magnitude() const {
        return sqrt(x * x + y * y);
    }

    float distanceSqr(const Vec2& other) const {
        int dx = x - other.x;
        int dy = y - other.y;
        return dx * dx + dy * dy;
    }
    string toString() const {
        return to_string(int(x)) + " " + to_string(int(y));
    }
};

class CheckpointManager {
private:
    vector<Vec2> checkpoints;
    int lap;
    int currentCheckpointIndex;
    int bestBoostIndex;

public:
    CheckpointManager() : lap(0), currentCheckpointIndex(0), bestBoostIndex(-1) {}

    void update(const Vec2& checkpoint) {
        if (checkpoints.empty()) {
            checkpoints.push_back(checkpoint);
            return;
        }

        if (checkpoint == checkpoints[currentCheckpointIndex]) return;

        if (checkpoint == checkpoints[0]) {
            if (lap == 0) {
                computeBestBoostIndex();
            }
            ++lap;
            currentCheckpointIndex = 0;
        } else {
            ++currentCheckpointIndex;
            if (lap == 0) {
                checkpoints.push_back(checkpoint);
            }
        }
    }

    int shouldUseBoost() const {
        return currentCheckpointIndex == bestBoostIndex;
    }

private:
    void computeBestBoostIndex() {
        if (checkpoints.size() < 2) {
            cerr << "Forgot to collect the checkpoints";
            return;
        }

        float longestDistance = 0.0f;
        size_t bestIndex = 0;

        for (size_t i = 0; i < checkpoints.size(); ++i) {
            size_t j = (i + 1) % checkpoints.size();
            float distance = checkpoints[i].distanceSqr(checkpoints[j]);
            if (distance > longestDistance) {
                longestDistance = distance;
                bestIndex = j;
            }
        }

        bestBoostIndex = static_cast<int>(bestIndex);
    }
};

class Pod {
public:
    Vec2 position;
    int thrust;
    bool boostUsed;

    Pod() : thrust(100), boostUsed(true) {}

    void update(int x, int y) {
        position = Vec2(x, y);
    }

    void boost() {
        boostUsed = false;
        cout << position.toString() << " " << boostExecuteKeyword << endl;
    }

    void move(int thrust) {
        cout << position.toString() << " " << thrust << endl;
    }
};


int main() {
    Pod myPod;
    CheckpointManager checkpointManager;
    Vec2 prevPos;

    while (true) {
        int x, y, next_checkpoint_x, next_checkpoint_y, next_checkpoint_dist, next_checkpoint_angle;
        cin >> x >> y >> next_checkpoint_x >> next_checkpoint_y >> next_checkpoint_dist >> next_checkpoint_angle;
        int opponent_x, opponent_y;
        cin >> opponent_x >> opponent_y;

        myPod.update(next_checkpoint_x, next_checkpoint_y);
        Vec2 nextCheckpoint(next_checkpoint_x, next_checkpoint_y);
        checkpointManager.update(nextCheckpoint);

        int thrust = (abs(next_checkpoint_angle) > BOOST_ANGLE_THRESHOLD) ? 0 : MAX_THRUST;

        if (checkpointManager.shouldUseBoost() && myPod.boostUsed) {
            myPod.boost();
        } else {
            myPod.move(thrust);
        }
    }
}

