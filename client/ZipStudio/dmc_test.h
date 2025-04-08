
#pragma once

#include <cstdint>
#include <cstddef>
#include <iostream>
#include <iomanip>      // For std::setprecision(n)
#include <vector>
#include <map>
#include <cmath>

// Structure representing a DMC state
struct DMCState {
    static constexpr std::size_t npos = static_cast<std::size_t>(-1);

    std::size_t count0;  // Count of 0 transitions
    std::size_t count1;  // Count of 1 transitions
    std::size_t next0;   // Next state when input is 0
    std::size_t next1;   // Next state when input is 1
    
    DMCState() : count0(1), count1(1), next0(npos), next1(npos) {}

    DMCState(std::size_t c0, std::size_t c1, std::size_t n0, std::size_t n1)
        : count0(c0), count1(c1), next0(n0), next1(n1) {}
};

class DMCompressor {
public:
    static constexpr std::size_t npos = static_cast<std::size_t>(-1);

private:
    std::vector<DMCState> states;   // Collection of all states
    std::size_t currentState;       // Current state index
    double threshold;               // Threshold for state cloning
    
public:
    DMCompressor(double cloneThreshold = 1.05) 
        : currentState(0), threshold(cloneThreshold) {
        // Initialize with a starting state
        DMCState state0{1, 0, 0, 1};
        states.push_back(state0);

        DMCState state1{0, 1, 0, 1};
        states.push_back(state1);
    }

    std::size_t getCurrentState() const {
        return currentState;
    }
    
    // Process one bit and update the model
    void processBit(int bit) {
        DMCState & state = states[currentState];
        
        // Update the appropriate counter
        if (bit == 0) {
            state.count0++;
        } else {
            state.count1++;
        }
        
        // Check if we need to clone the state
        checkForClone(currentState, bit);
        
        // Transition to next state
        currentState = (bit == 0) ? state.next0 : state.next1;
        
        // Create new state if transition doesn't exist
        if (currentState == npos) {
            currentState = states.size();
            states.push_back(DMCState());
            if (bit == 0) {
                state.next0 = currentState;
            } else {
                state.next1 = currentState;
            }
        }
    }
    
    // Get probability of 0 in current state
    double getProbability0() const {
        const DMCState & state = states[currentState];
        return (double)state.count0 / (state.count0 + state.count1);
    }
    
    // Reset to initial state
    void reset() {
        currentState = 0;
    }
    
    // Get total number of states (for demonstration)
    size_t getStateCount() const {
        return states.size();
    }
    
private:
    // Check if state needs to be cloned
    void checkForClone(std::size_t stateIdx, int bit) {
        DMCState & state = states[stateIdx];
        std::size_t nextState = (bit == 0) ? state.next0 : state.next1;
        
        if (nextState == npos) return;
        
        DMCState & next = states[nextState];
        double ratio = (bit == 0) 
            ? (double)state.count0 / next.count0 
            : (double)state.count1 / next.count1;
        
        // Clone state if ratio exceeds threshold
        if (ratio > threshold) {
            cloneState(stateIdx, bit);
        }
    }
    
    // Perform state cloning
    void cloneState(std::size_t stateIdx, int bit) {
        DMCState & state = states[stateIdx];
        std::size_t oldNext = (bit == 0) ? state.next0 : state.next1;
        
        //std::cout << "cloneState() Enter." << std::endl;

        if (oldNext == npos) return;
        
        // Create new state as a clone
        DMCState newState = states[oldNext];
        
        // Adjust counts to prevent over-specialization
        newState.count0 = (newState.count0 + 1) / 2;
        newState.count1 = (newState.count1 + 1) / 2;
        
        // Add the new state
        std::size_t newNext = states.size();
        states.push_back(newState);
        
        // Update the transition
        if (bit == 0) {
            state.next0 = newNext;
        } else {
            state.next1 = newNext;
        }

        //std::cout << "cloneState() Over." << std::endl;
    }
};
