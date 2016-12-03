#pragma once
#include <vector>

// Define an interface for AI players to manage possible
// goals of their strategy.

class AbstractGoal
{
public:
    struct DeployTo {
        // a territory we should deploy if we pursue this goal
        int territory; // territory number
        int armies; // ideal number of armies to place here
    };

    AbstractGoal();
    virtual float feasibility()=0; // calculate feasibility of this goal
    // 0..1 probability for this turn,

    std::vector<DeployTo> deploys;
};

