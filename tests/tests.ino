#include <AUnit.h>    // Testing
#include <WiFi.h>     // ESP32
#include "chat.h"     
#include "secrets.h"  // Network name, password, and private API key

using namespace ChatGPTuino;

#define TESTING_ON 1

/* Assert( expected value (Known Value), actual value(value under test)) */

const char *test_key = "sk-VT65uEtK8cUfB1KuEx0QT3BlbkFJHnIvsADF3rJw5-XXXXXX";
const char *model = "gpt-3.5-turbo";

#if TESTING_ON
test(ChatBox_itializes_with_valid_values) {

  ChatBox chat{ 0, 0 };
  chat.init(test_key, model);
  long testDocSize = 3056; //Based on Arduino JSON 6 assistant
  assertEqual((const char *)model, (const char *)chat.model());
  assertEqual((long)chat.maxTokens(), (long)MIN_TOKENS);
  assertEqual(chat.numMessages(), MIN_MESSAGES);
  assertEqual((long)(CHARS_PER_TOKEN * chat.maxTokens()), (long)chat.MAX_MESSAGE_LENGTH());
  assertEqual(testDocSize, (long)chat.DYNAMIC_JSON_DOC_SIZE());
}

test(init_allocates_space_for_message_contexts) {

  ChatGPTuino::ChatBox chat{ 10, 5 };
  chat.init(test_key, model);

  for (int i = 0; i < chat.numMessages() - 1; i++) {

    long contentPtrA = (long)chat.contentPtrs(i);
    long contentPtrB = (long)chat.contentPtrs(i + 1);
    long actual = contentPtrB - contentPtrA;
    long expected = chat.MAX_MESSAGE_LENGTH() * sizeof(char);

    assertEqual(expected, actual);
  }
}

test(putMessage_puts_message_in_next_available_slot) {

  ChatGPTuino::ChatBox chat{ 10, 4 };
  chat.init(test_key, model);

  char *testMessage = "testing_1";
  chat.putMessage(testMessage, strlen(testMessage));

  assertEqual((const char *)testMessage, (const char *)chat.getLastMessageContent());

  char *testMessage2 = "testing_2";
  chat.putMessage(testMessage2, strlen(testMessage2));

  assertEqual((const char *)testMessage2, (const char *)chat.getLastMessageContent());
}

test(putMessage_assigns_default_role_of_message_to_user) {
  ChatGPTuino::ChatBox chat{ 10, 4 };
  chat.init(test_key, model);

  char *testMessage = "testing_1";
  chat.putMessage(testMessage, strlen(testMessage));

  assertEqual(1, chat.getLastMessageRole());
}

test(putMessage_assigns_specified_role_to_message) {
  ChatGPTuino::ChatBox chat{ 10, 4 };
  chat.init(test_key, model);

  char *testMessage = "testing_1";
  chat.putMessage(testMessage, strlen(testMessage), ChatGPTuino::Roles::assistant);

  assertEqual(ChatGPTuino::Roles::assistant, chat.getLastMessageRole());
}

test(generateJsonRequestBody_returns_valid_Json) {
  ChatGPTuino::ChatBox chat{ 10, 4 };
  chat.init(test_key, model);

  char *testMessage = "JSON body testing";
  chat.putMessage(testMessage, ChatGPTuino::Roles::user);

  DynamicJsonDocument testDoc = chat.generateJsonRequestBody();

  assertEqual((const char *)model, (const char *)testDoc["model"]);
  assertEqual(chat.maxTokens(), testDoc["max_tokens"]);
  assertEqual((const char *)chat.getLastMessageContent(), (const char *)testDoc["messages"][0]["content"]);

  char *testMessage2 = "JSON body testing2";
  chat.putMessage(testMessage2, strlen(testMessage2), ChatGPTuino::Roles::user);
  testDoc = chat.generateJsonRequestBody();

  assertEqual((const char *)chat.getLastMessageContent(), (const char *)testDoc["messages"][1]["content"]);
}

test(getResponse_puts_response_in_messages) {
  ChatGPTuino::ChatBox chat{ 10, 4 };
  chat.init(openAI_Private_key, model);

  char *testMessage = "Please respond with the only the word TEST";
  chat.putMessage(testMessage, strlen(testMessage),ChatGPTuino::Roles::user);
  chat.getResponse();

  assertEqual("TEST", (const char *)chat.getLastMessageContent());
}

test(getResponse_puts_response_length_in_messages) {
  ChatGPTuino::ChatBox chat{ 10, 4 };
  chat.init(openAI_Private_key, model);

  char *testMessage = "Please respond with the only the word TEST";
  chat.putMessage(testMessage,strlen(testMessage), ChatGPTuino::Roles::user);
  chat.getResponse();

  assertEqual((long)4, (long)chat.getLastMessageLength());
}


#endif

void setup() {
  Serial.begin(115200);

  // WiFi Setup
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("WiFi connected to IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {

#if TESTING_ON
  aunit::TestRunner::run();
#else
  static bool runOnce = true;
  if (runOnce) {
    helloChatGPT();
    runOnce = false;
  }
#endif
}
