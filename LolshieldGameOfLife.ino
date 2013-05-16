#include <Charliplexing.h> // Import LOL Shield library, initialize this in setup()

int blinkdelay = 250;     // Sets the time each frame is shown
int resetDelay = 5000;    // How long to wait before re-initializing when there was no change
int iterationCounter = 0; // counts how many generations have been simulated
int maxLength = 720;      // After this many generations the simulation is terminated (useful for killing boring oscillators)

byte prev[9*14];          // Holds the previous generation from which the next state is computed
byte curr[9*14];          // The new generation is written into here, it will become the previous generation sooner than it would like


void setup() {
  //Initializes the screen
  LedSign::Init();  
  // initialize the random number generator
  randomSeed(analogRead(0)); 
  // populate the world with random individuals
  initializeWorld();
}

void loop() {
  // Do one update step in Conway's Game of Life
  byte wasThereAnyChange = DoGameOfLifeStep();
  // If there was no change between the last two time steps
  // or if we have reached the maximal duration, restart
  if ( wasThereAnyChange == 0 || iterationCounter++ > maxLength) {
    initializeWorld();
    iterationCounter = 0;
    delay(resetDelay);
  } 
  // otherwise just wait a little bit so that viewers'
  // brains can process what's going on before the next 
  // time step gets shown
  else {
    delay(blinkdelay); 
  }
}


// returns any pixel from the previous generation given
// line and column count, takes care of wraparound
byte getPrev (int led, int line) {
  while (led < 0) led += 13; // there's probably a better (maybe bitshifty?) way of doing this, think about it some time
  if (led > 13) led = led % 13;
  while (line < 0) line += 9; // see comment two lines above
  if (line > 8) line = line % 9;
  
  return prev[line*14+led];
}

// sets the entry in curr[], makes sure only 1 and 0 get in
void setCurr (int led, int line, byte val) {
  if (val > 0) val = 1;
  if (val < 0) val = 0;
  curr[line*14+led] = val;
}

// copies all entries from curr to prev while checking
// if there are any changes between the two
byte copyCurrToPrev () {
  byte change = 0;
  
  for(byte line = 0; line < 9; line++) {
    for (byte led=0; led<14; ++led) {
      // if the (old) previous and current entries
      // are different, the world has changed
      if (prev[line*14+led] != curr[line*14+led]) change = 1;
      
      // move the (old) current into the (new) previous
      prev[line*14+led] = curr[line*14+led];
    }
  }
  
  return change;
}

// insert random values into the world, probability
// of life in each cell is 0.5.
// change this function to start with interesting creatures
// instead of random noise.
void initializeWorld () {
  // initialize prev with random entries
  for(byte line = 0; line < 9; line++) {
    for (byte led=0; led<14; ++led) {
      setCurr(led, line, random(0,2));
    }
  }
  copyCurrToPrev();  
}

// this function is called at every time step
// it makes the game-of-lify update and also 
// switches the LEDs
byte DoGameOfLifeStep () {
  for(int line = 0; line < 9; line++) {
    for (int led=0; led<14; ++led) {

      byte myself = getPrev(led, line);
      byte sumOfNeighbors = 0;
      
      sumOfNeighbors += getPrev(led-1, line-1);
      sumOfNeighbors += getPrev(led-1, line);
      sumOfNeighbors += getPrev(led-1, line+1);
      sumOfNeighbors += getPrev(led, line-1);
      sumOfNeighbors += getPrev(led, line+1);
      sumOfNeighbors += getPrev(led+1, line-1);
      sumOfNeighbors += getPrev(led+1, line);
      sumOfNeighbors += getPrev(led+1, line+1);
      
      // the next line is where the game of life rules are implemneted
      // if you don't like the way the world works, edit it!
      if ( (myself == 1 && sumOfNeighbors > 1 && sumOfNeighbors < 4) || (myself == 0 && sumOfNeighbors == 3) ) {
        LedSign::Set(led, line, 1);
        setCurr(led, line, 1);
      } else {
        LedSign::Set(led, line, 0);
        setCurr(led, line, 0);
      }
    }
  }
  
  // the return value of "copyCurrToPrev" is 1 when
  // the world has changed, otherwise 0
  return copyCurrToPrev();
}
