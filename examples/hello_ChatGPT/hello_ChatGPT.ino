#include <ChatGPTuino.h>
#include <WiFi.h>     // ESP32
#include "credentials.h"  // Network name, password, and private API key

// A quick primer on the chatGPT API https://www.programmingelectronics.com/chatgpt-api/
const int TOKENS = 100; // How lengthy a response you want, every token is about 3/4 a word
const int NUM_MESSAGES = 20; 

ChatGPTuino chat{ TOKENS, NUM_MESSAGES }; // Will store and send your most recent messages (up to NUM_MESSAGES)
const char *model = "gpt-4o";  // OpenAI Model being used

void setup() {

  Serial.begin(115200); // Start serial communication
  
  // WiFi Setup
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Initialize messages array
  chat.init(key, model);

  char *userMessage = "Hello ChatGPT";  // User message to ChatGPT
  
  // Add message to messages array
  chat.putMessage(userMessage, strlen(userMessage));
  
  // Send message array and receive response
  chat.getResponse();

  // Print response
  Serial.println(chat.getLastMessageContent());
}

void loop() {
}