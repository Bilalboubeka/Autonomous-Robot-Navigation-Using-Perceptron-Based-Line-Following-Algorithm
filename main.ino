#include <Arduino.h>

// Define pins for IR sensors
const int irSensor1 = A0;
const int irSensor2 = A1;
const int irSensor3 = A2;

// Define pins for motor control
const int motorLeftForward = 9;
const int motorLeftBackward = 8;
const int motorRightForward = 7;
const int motorRightBackward = 6;

// Define the size of the input
const int inputSize = 3;
const float learningRate = 0.01;
const int epochs = 100;

// Weights and bias
float weights[inputSize] = {0.0, 0.0, 0.0};
float bias = 0.0;

// Training data
float X[8][inputSize] = {
    {0, 0, 0},
    {0, 0, 1},
    {0, 1, 0},
    {0, 1, 1},
    {1, 0, 0},
    {1, 0, 1},
    {1, 1, 0},
    {1, 1, 1}
};
int y[8] = {0, 0, 0, 1, 0, 1, 1, 1};  // Example labels: 0 for left, 1 for right

// Step function
int stepFunction(float x) {
    return x >= 0 ? 1 : 0;
}

// Predict function
int predict(float x[]) {
    float linearOutput = bias;
    for (int i = 0; i < inputSize; i++) {
        linearOutput += weights[i] * x[i];
    }
    return stepFunction(linearOutput);
}

// Train function
void train(float X[][inputSize], int y[], int epochs) {
    for (int epoch = 0; epoch < epochs; epoch++) {
        for (int i = 0; i < 8; i++) {
            int prediction = predict(X[i]);
            int error = y[i] - prediction;

            // Update weights and bias
            for (int j = 0; j < inputSize; j++) {
                weights[j] += learningRate * error * X[i][j];
            }
            bias += learningRate * error;
        }
    }
}

// Function to control the motors
void turnLeft() {
    digitalWrite(motorLeftForward, LOW);
    digitalWrite(motorLeftBackward, HIGH);
    digitalWrite(motorRightForward, HIGH);
    digitalWrite(motorRightBackward, LOW);
}

void turnRight() {
    digitalWrite(motorLeftForward, HIGH);
    digitalWrite(motorLeftBackward, LOW);
    digitalWrite(motorRightForward, LOW);
    digitalWrite(motorRightBackward, HIGH);
}

void stopMotors() {
    digitalWrite(motorLeftForward, LOW);
    digitalWrite(motorLeftBackward, LOW);
    digitalWrite(motorRightForward, LOW);
    digitalWrite(motorRightBackward, LOW);
}

void setup() {
    // Start the serial communication
    Serial.begin(9600);

    // Initialize motor control pins
    pinMode(motorLeftForward, OUTPUT);
    pinMode(motorLeftBackward, OUTPUT);
    pinMode(motorRightForward, OUTPUT);
    pinMode(motorRightBackward, OUTPUT);

    // Train the perceptron
    train(X, y, epochs);
}

void loop() {
    // Read inputs from IR sensors
    float sensorInput[inputSize] = {
        analogRead(irSensor1) > 512 ? 1.0 : 0.0,
        analogRead(irSensor2) > 512 ? 1.0 : 0.0,
        analogRead(irSensor3) > 512 ? 1.0 : 0.0
    };

    // Make a prediction
    int prediction = predict(sensorInput);

    // Control the robot based on prediction
    if (prediction == 0) {
        turnLeft();
    } else {
        turnRight();
    }

    // Add a small delay to avoid rapid switching
    delay(100);
}
