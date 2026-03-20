#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "GameController.h"
#include "UserAction.h"

using namespace wfrest;
using namespace ctl;
using namespace std;

GameController& controller = GameController::getInstance("default");

// -- DON'T MODIFY ANY CODE ABOVE THIS LINE -- //
// -- YOU ARE ALLOW TO ADD HEADER FILES UNDER THIS LINE -- //

// TODO: Choose your recipe mode
// you may want to have different strategy for different recipe mode

//const string RecipeMode = "Salad";
//const string RecipeMode = "SaladAndCheeseBurger";
 const string RecipeMode = "AllRecipe";


// A template GLOBAL VARIABLE vector to store operations
// Feel free to modify this data structure! (or create your own to use)
vector<string> operations;


// A template map to store the position of each counter
// Question: How do you extend this map to store more than one position for a counter?
// Question2: cutting / normal counter have more than one position, how will you handle it?
const map<Counter, vector<pair<int, int>>> counterPosition = {
    { BreadCounter, {{0, 1}} },
    { CabbageCounter, {{8, 4}} },
    { CheeseBlockCounter, {{0, 10}} },
    { TomatoCounter, {{8, 7}} },
    { RawPattyCounter, {{0, 20}} },
    { StoveCounter, {{0, 19}} },
    { PlatesCounter, {{2, 20}, {4, 20}} },
    { TrashCounter, {{6, 20}} },
    { DeliveryCounter, {{0, 20}, {1, 20}} },

    // There are 2 cutting counters, this is only one of it
    { CuttingCounter, {{8, 14}, {0, 14}} },
    // There are so many normal counters, this is only one of it
    { NormalCounter, {{8, 13}, {0, 13}} }
};

const map<Counter, string> counterDirection = {
    { BreadCounter, "w" },
    { CabbageCounter, "s" },
    { CheeseBlockCounter, "w" },
    { TomatoCounter, "s" },
    { RawPattyCounter, "w" },
    { StoveCounter, "w" },
    { PlatesCounter, "d" },
    { TrashCounter, "d" },
    { DeliveryCounter, "d" },

    // There are 2 cutting counters, this is only one of it
    { CuttingCounter, "s" },
    // There are so many normal counters, this is only one of it
    { NormalCounter, "s" }
};

void DefaultInitialize();
void DefaultSendOperation();
void SaladSendOperation();
void SaladInitialize();



// Init the game (DO NOT MODIFY THIS FUNCTION)
void UserAction::InitGame() {
    Initialize();
    // Set the response to "ok" when finished initialization
    controller.SetResponse("ok");
}

// Just a cute Initializing function
void UserAction::Initialize() {
    // TODO: You can do some initialization in this function.
    // Feel free to modify this function.
    // DefaultInitialize() will make you a MEGABurger!
    cout << "Initializing the game..." << endl;
    //DefaultInitialize();
    SaladInitialize();
}

// Main Function of you game logic
void UserAction::SendOperation() {
    // TODO: Implement your gaming logic here
    // DefaultSendOperation() will make you a MEGABurger!
    SaladSendOperation();
    //DefaultSendOperation();
}


// -- Moving series functions Below -- //

void MovePointToPoint(pair<int, int> from, pair<int, int> to) {
    //  Move from one point to another point
    string xmoves,ymoves;
    if(to.first>from.first){
        xmoves = "s";
    }
    else{
        xmoves = "w";
    }
    if(to.second>from.second){
        ymoves = "d";
    }
    else{
        ymoves = "a";
    }
    int xcnt,ycnt;
    xcnt = abs(from.first - to.first);
    ycnt = abs(from.second - to.second);

    for(int i=0;i<xcnt;i++){
        operations.push_back(xmoves);
    }
    for(int i=0;i<ycnt;i++){
        operations.push_back(ymoves);
    }

}

void MovePointToCounter(pair<int, int> fromPoint, Counter toCounter, int whichcounter) {
    //  Move from one point to a counter
    vector<pair<int, int> > targetPosition = counterPosition.at(toCounter);
    MovePointToPoint(fromPoint,targetPosition[whichcounter]);

}

void MoveCounterToCounter(Counter from, int whereyouare, Counter to,int whichcounter) {
    //  Move from one counter to another counter
    vector<pair<int,int> > fromPosition = counterPosition.at(from);
    MovePointToCounter(fromPosition[whereyouare],to,whichcounter);
}
void MoveCounterToCounterAndInteract(Counter from, int whereyouare, Counter to,int whichcounter){
    MoveCounterToCounter(from,whereyouare, to,whichcounter);
    operations.push_back(counterDirection.at(to));
    operations.push_back("e");
}
void Interact(Counter To){
    operations.push_back(counterDirection.at(To));
    operations.push_back("e");
}
// -- Moving series functions Above -- //

// -- Strategy for different Recipe Mode Below -- //

Recipe SaladModeStrategy() {
    // Only salad will be in order in Salad Mode
    return Salad;
}

Recipe SaladCheeseburgerModeStrategy() {
    // TODO: Implement your salad cheeseburger mode strategy here
    // Below is a simple example, feel free to change it

    vector<Order> orderList = controller.GetOrderList();
    if (orderList.empty()) {
        return Salad;
    }
    return orderList[0].recipe;
}

Recipe AllRecipeModeStrategy() {
    // TODO: Implement your all recipe mode strategy here
    // Below is a simple example, feel free to change it
    vector<Order> orderList = controller.GetOrderList();
    if (orderList.empty()) {
        return Salad;
    }
    return orderList[0].recipe;
}

Recipe GetNextOrder() {
    // Return the next order based on the Recipe Mode
    if (RecipeMode == "Salad") {
        return SaladModeStrategy();
    } else if (RecipeMode == "SaladAndCheeseBurger") {
        return SaladCheeseburgerModeStrategy();
    } else {
        return AllRecipeModeStrategy();
    }
}

// -- Strategy for different Recipe Mode Above -- //

// -- Miscallaneous functions Below -- //

void CutIngredient(int times) {
    // Cut the Ingredient for times
    for(int i=0;i<times;i++){
        operations.push_back("f");
    }
}

// -- Miscallaneous functions Above -- //

// -- Pipeline Funtions Below -- //

// You are welcome to change the function prototype
// Like changing the return type or adding more parameters
void MakeSalad() {
    //  Salad Making Pipeline
    MovePointToCounter(controller.GetPlayerPosition(),PlatesCounter,1);
    Interact(PlatesCounter);
    MoveCounterToCounterAndInteract(PlatesCounter,1,NormalCounter,0);
    MoveCounterToCounterAndInteract(NormalCounter,0,TomatoCounter,0);
    MoveCounterToCounterAndInteract(TomatoCounter,0,CuttingCounter,0);
    CutIngredient(3);
    Interact(CuttingCounter);
    MoveCounterToCounterAndInteract(CuttingCounter,0,NormalCounter,0);
    MoveCounterToCounterAndInteract(NormalCounter,0,CabbageCounter,0);
    MoveCounterToCounterAndInteract(CabbageCounter,0,CuttingCounter,0);
    CutIngredient(5);
    Interact(CuttingCounter);
    MoveCounterToCounterAndInteract(CuttingCounter,0,NormalCounter,0);
    Interact(NormalCounter);
    MoveCounterToCounterAndInteract(NormalCounter,0,DeliveryCounter,1);
    
}
void MakeBurger() {
    MovePointToCounter(controller.GetPlayerPosition(),RawPattyCounter,0);
    Interact(RawPattyCounter);
    MoveCounterToCounterAndInteract(RawPattyCounter,0,StoveCounter,0);
    MoveCounterToCounterAndInteract(StoveCounter,0,PlatesCounter,0);
    MoveCounterToCounter(PlatesCounter,0,NormalCounter,1);
    Interact(StoveCounter);
    MoveCounterToCounterAndInteract(NormalCounter,1,BreadCounter,0);
    MoveCounterToCounter(BreadCounter,0,NormalCounter,1);
    Interact(StoveCounter);
    Interact(StoveCounter);
    Interact(StoveCounter);
    Interact(StoveCounter);
    Interact(StoveCounter);
    Interact(StoveCounter);
    Interact(StoveCounter);
    Interact(StoveCounter);
    MoveCounterToCounterAndInteract(NormalCounter,1,StoveCounter,0);
    MoveCounterToCounterAndInteract(StoveCounter,0,DeliveryCounter,0);
}
void MakeCheeseBurger() {
    MovePointToCounter(controller.GetPlayerPosition(),RawPattyCounter,0);
    Interact(RawPattyCounter);
    MoveCounterToCounterAndInteract(RawPattyCounter,0,StoveCounter,0);
    MoveCounterToCounterAndInteract(StoveCounter,0,PlatesCounter,0);
    MoveCounterToCounter(PlatesCounter,0,NormalCounter,1);
    Interact(StoveCounter);
    MoveCounterToCounterAndInteract(NormalCounter,1,BreadCounter,0);
    MoveCounterToCounter(BreadCounter,0,NormalCounter,1);
    Interact(StoveCounter);
    MoveCounterToCounterAndInteract(NormalCounter,1,CheeseBlockCounter,0);
    MoveCounterToCounter(CheeseBlockCounter,0,CuttingCounter,1);
    Interact(StoveCounter);
    CutIngredient(3);
    Interact(StoveCounter);
    MoveCounterToCounter(CuttingCounter,1,NormalCounter,1);
    Interact(StoveCounter);
    Interact(StoveCounter);
    MoveCounterToCounterAndInteract(NormalCounter,1,StoveCounter,0);
    MoveCounterToCounterAndInteract(StoveCounter,0,DeliveryCounter,0);

}

void MakeMegaBurger() {
    MovePointToCounter(controller.GetPlayerPosition(),RawPattyCounter,0);
    Interact(RawPattyCounter);
    MoveCounterToCounterAndInteract(RawPattyCounter,0,StoveCounter,0);
    MoveCounterToCounterAndInteract(StoveCounter,0,PlatesCounter,0);
    MoveCounterToCounter(PlatesCounter,0,NormalCounter,1);
    Interact(StoveCounter);
    MoveCounterToCounterAndInteract(NormalCounter,1,BreadCounter,0);
    MoveCounterToCounter(BreadCounter,0,NormalCounter,1);
    Interact(StoveCounter);
    MoveCounterToCounterAndInteract(NormalCounter,1,CheeseBlockCounter,0);
    MoveCounterToCounter(CheeseBlockCounter,0,CuttingCounter,1);
    Interact(StoveCounter);
    CutIngredient(3);
    Interact(StoveCounter);
    MoveCounterToCounter(CuttingCounter,1,NormalCounter,1);
    Interact(StoveCounter);
    Interact(StoveCounter);
    MoveCounterToCounterAndInteract(NormalCounter,1,StoveCounter,0);
    MoveCounterToCounterAndInteract(StoveCounter,0,NormalCounter,0);
    MoveCounterToCounterAndInteract(NormalCounter,0,TomatoCounter,0);
    MoveCounterToCounterAndInteract(TomatoCounter,0,CuttingCounter,0);
    CutIngredient(3);
    Interact(CuttingCounter);
    MoveCounterToCounterAndInteract(CuttingCounter,0,NormalCounter,0);
    MoveCounterToCounterAndInteract(NormalCounter,0,CabbageCounter,0);
    MoveCounterToCounterAndInteract(CabbageCounter,0,CuttingCounter,0);
    CutIngredient(5);
    Interact(CuttingCounter);
    MoveCounterToCounterAndInteract(CuttingCounter,0,NormalCounter,0);
    Interact(NormalCounter);
    MoveCounterToCounterAndInteract(NormalCounter,0,DeliveryCounter,1);
    
    
}



// -- Pipeline Funtions Below -- //


void SaladSendOperation(){
    string s = "";
    
    if (operations.empty()) {
        Recipe nextRecipe = GetNextOrder();
        if (controller.GetRound() <= 20){
            
            MovePointToCounter(controller.GetPlayerPosition(),TrashCounter,0);
            MoveCounterToCounter(TrashCounter,0,PlatesCounter,1);
            
        }
        if (nextRecipe == Salad){
            MakeSalad();
        }
        else if (nextRecipe == Burger){
            MakeBurger();
        }
        else if (nextRecipe == CheeseBurger){
            MakeCheeseBurger();
        }
        else if (nextRecipe == MegaBurger){
            MakeMegaBurger();
        }
    }
    reverse(operations.begin(),operations.end());
    s = operations.back();
    operations.pop_back();
    reverse(operations.begin(),operations.end());

    if (s == "w") controller.MoveUp();
    if (s == "s") controller.MoveDown();
    if (s == "a") controller.MoveLeft();
    if (s == "d") controller.MoveRight();
    if (s == "e") controller.Interact();
    if (s == "f") controller.InteractSpecial();
}

void SaladInitialize(){
    while (!operations.empty())
    {
        operations.pop_back();
    }
    
}

// -- Default Series Function Below -- //

// SendOperation function template, free MEGABurger for you!

void DefaultSendOperation() {
    string s = "";
    if (!operations.empty()) {
        s = operations.back();
        operations.pop_back();
        cout << "Operation: " << s << endl;
    }
    if (s == "w") controller.MoveUp();
    if (s == "s") controller.MoveDown();
    if (s == "a") controller.MoveLeft();
    if (s == "d") controller.MoveRight();
    if (s == "e") controller.Interact();
    if (s == "f") controller.InteractSpecial();
}