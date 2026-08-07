# Real-Time Embedded Traffic Control System using SysTick, Finite State Machines and Bare-Metal STM32

## 📌 Project Overview

This project is a **real-time traffic light controller built from scratch using an STM32 microcontroller**.

I built it to move beyond simply making LEDs turn ON and OFF. My main goal was to understand how a microcontroller can continuously manage **multiple timed states without blocking the CPU**.

The project uses:

* Bare-metal STM32 register programming
* GPIO registers
* SysTick timer
* 1 ms system time base
* Finite State Machine (FSM)
* Non-blocking timing
* Elapsed-time scheduling
* `volatile` variables
* C `enum` for state management

I intentionally did **not** use `HAL_Delay()` for the traffic-light timing.

---

# 🧠 What I Was Trying to Understand

The main question behind this project was:

> **How can I make a microcontroller control a timed process without stopping the CPU while it waits?**

A simple approach would be:

```c
LED_ON();
HAL_Delay(5000);
LED_OFF();
```

But that approach blocks execution.

Instead, I wanted the microcontroller to continuously run and simply check:

```c
Has enough time passed?
```

If the answer is no, it keeps running.

If the answer is yes, it changes to the next state.

That became the foundation of this project.

---

## Project Code 

[Click Here to check out the project code](code)

## Project images
[Click here to check out the project images](images/)

## Project Demo Video
[Click here to check out the project Demostration Video](https://youtu.be/pjeuZYLHlPI)

# 🚦 Traffic Light Design

I divided the intersection into two roads.

### Road A

| Function | STM32 Pin |
| -------- | --------- |
| Red      | PB0       |
| Yellow   | PB1       |
| Green    | PB2       |

### Road B

| Function | STM32 Pin |
| -------- | --------- |
| Red      | PA3       |
| Yellow   | PA4       |
| Green    | PA5       |

The controller follows six states:

```text
STATE_A_GREEN
       ↓
STATE_A_YELLOW
       ↓
ALL_RED_1
       ↓
STATE_B_GREEN
       ↓
STATE_B_YELLOW
       ↓
ALL_RED_2
       ↓
STATE_A_GREEN
       ↓
       ...
```

This creates a continuous traffic-light cycle.

---

# 🔄 How the State Machine Works

One of the most important things I learned from this project is how to think about a system as a **Finite State Machine**.

Instead of thinking:

> "Turn this LED on, wait, turn this LED off..."

I started thinking:

> "What state is the system currently in, and what state should come next?"

For example:

```text
Road A Green
    ↓
5 seconds elapsed?
    ↓ YES
Road A Yellow
    ↓
2 seconds elapsed?
    ↓ YES
All Red
    ↓
1 second elapsed?
    ↓ YES
Road B Green
```

Each state has:

1. A defined LED configuration.
2. A defined duration.
3. A defined next state.

This made the behavior much easier to reason about.

---

# 🧩 Why I Used `enum`

I used a C `enum` to represent the traffic states:

```c
typedef enum
{
    STATE_A_GREEN,
    STATE_A_YELLOW,
    ALL_RED_1,
    STATE_B_GREEN,
    STATE_B_YELLOW,
    ALL_RED_2

} TrafficState;
```

Before working with state machines, I mainly thought of an `enum` as simply a way of assigning names to numbers.

Here I understood its usefulness more clearly.

Instead of writing something unclear like:

```c
currentState = 0;
```

I can write:

```c
currentState = STATE_A_GREEN;
```

The code now describes the actual behavior of the system.

The `enum` therefore gives my state machine **meaningful and readable states**.

---

# ⏱️ How I Used SysTick

The controller needs a reliable way to measure time.

I used the Cortex-M SysTick timer to generate a **1 ms time base**:

```c
SysTick_Config(16000);
```

With the configured system clock, the SysTick interrupt occurs every 1 ms.

The interrupt updates:

```c
systemTime++;
```

So `systemTime` acts like a continuously running software stopwatch:

```text
1 ms
2 ms
3 ms
4 ms
5 ms
...
```

This allowed the main program to determine how long it had been in a particular traffic state.

---

# 🕐 Understanding `stateStartTime`

This was another important part of the project for me.

I needed to know:

> "When did the current state begin?"

So I stored the current system time:

```c
stateStartTime = systemTime;
```

Then I calculate elapsed time using:

```c
systemTime - stateStartTime
```

For example, if:

```text
stateStartTime = 10000 ms
systemTime     = 15000 ms
```

then:

```text
15000 - 10000 = 5000 ms
```

Therefore, 5 seconds have passed.

The controller can then transition to the next state.

---

# 🚫 Why I Didn't Use `HAL_Delay()`

One of the main lessons from this project was understanding **blocking versus non-blocking execution**.

A blocking approach would look like:

```c
HAL_Delay(5000);
```

The processor essentially waits for the delay to finish before continuing.

Instead, my implementation continuously checks:

```c
if((systemTime - stateStartTime) >= 5000)
```

The CPU does not sit inside a five-second delay.

It keeps executing the main loop and checking the current state and elapsed time.

This is what made the project a useful exercise in **non-blocking scheduling**.

---

# 🔁 How the `while(1)` Works

The microcontroller needs to control the traffic lights continuously, so the application runs inside:

```c
while(1)
{
    ...
}
```

The loop repeatedly asks:

```text
What state am I in?
        ↓
Set the LEDs for that state
        ↓
Has the required time elapsed?
        ↓
NO → Keep running
        ↓
YES → Move to the next state
```

It does this continuously for as long as the microcontroller is powered.

---

# 🧠 The Overall Program Flow

The complete program can be understood as:

```text
Start
  ↓
Enable GPIOA and GPIOB clocks
  ↓
Configure LED pins as outputs
  ↓
Configure SysTick for 1 ms timing
  ↓
Set initial state
  ↓
Record state start time
  ↓
Enter while(1)
  ↓
Check current state
  ↓
Set appropriate LEDs
  ↓
Calculate elapsed time
  ↓
Has state duration expired?
  │
  ├── NO → Continue checking
  │
  └── YES → Change state
                ↓
         Save new start time
                ↓
         Continue forever
```

---

# 🔧 Bare-Metal Programming

Another major part of this project was deliberately working at the register level.

For example, instead of using a high-level GPIO function, I configured the GPIO mode directly:

```c
GPIOA->MODER &= ~(3 << (3 * 2));
GPIOA->MODER |=  (1 << (3 * 2));
```

And controlled the output directly through:

```c
GPIOA->ODR
```

and:

```c
GPIOB->ODR
```

This helped me understand that a GPIO pin is ultimately controlled by **hardware registers**, rather than magic functions.

I am beginning to see the relationship as:

```text
C code
   ↓
Peripheral Register
   ↓
STM32 Hardware
   ↓
GPIO Pin
   ↓
LED
```

That connection is one of the most valuable things I took from this project.

---

# ⚡ Understanding `volatile`

I used:

```c
extern volatile uint32_t systemTime;
```

because `systemTime` is modified by the SysTick interrupt while the main program is also reading it.

The `volatile` keyword tells the compiler:

> "This value can change unexpectedly, so don't assume its value remains unchanged."

This was important for understanding how **interrupt-driven systems interact with normal program execution**.

---

# 🛠️ What I Learned From Building This

This project helped me connect several concepts that I had previously studied separately.

### 1. GPIO Register Programming

I practiced configuring GPIO pins directly through `MODER` and controlling outputs through `ODR`.

### 2. Interrupt-Based Timing

I used SysTick to create a 1 ms system time base.

### 3. Finite State Machines

I learned how to represent a real physical process as a sequence of well-defined software states.

### 4. Non-Blocking Programming

I learned that timing does not always require stopping the processor and waiting.

### 5. Elapsed-Time Scheduling

I learned how to determine whether an event should occur by comparing timestamps.

### 6. `volatile`

I gained a practical understanding of why variables shared between interrupt code and normal code may need to be declared `volatile`.

### 7. Bare-Metal STM32 Programming

I strengthened my understanding of how software interacts directly with microcontroller peripheral registers.

---

# 💡 The Biggest Lesson

The biggest lesson from this project was that **embedded programming is not just about making something work**.

It is also about deciding **how the system should behave while it is working**.

A traffic light can be made to work with a sequence of delays.

But designing it as a state machine with a system timer made me think more about:

* timing,
* CPU availability,
* system states,
* transitions,
* interrupts,
* responsiveness,
* and future scalability.

That change in thinking was the main reason I built this project.

---

# 🚀 Possible Future Improvements

This project currently uses fixed timing, but the architecture can be extended.

Possible improvements include:

* Pedestrian crossing button
* Emergency vehicle priority
* Traffic sensors
* Dynamic green-light duration
* UART monitoring
* LCD/OLED status display
* Fault detection
* Multiple intersections
* Sensor-based adaptive traffic control

The goal would be to add these features **without replacing the fundamental state-machine architecture**.

---

# 🧰 Technologies & Concepts

```text
Microcontroller : STM32F401
Language        : C
Programming     : Bare-Metal / Register-Level
Timer           : SysTick
Architecture    : Finite State Machine
Scheduling      : Non-Blocking
GPIO            : Direct Register Access
Timing          : 1 ms System Time Base
```

---

# 📚 Final Reflection

This project was more than just making six LEDs behave like traffic lights.

It was an exercise in learning how to break a real-world process into **states, timing requirements, transitions, and hardware actions**, then implement that behavior directly on a microcontroller.

Most importantly, I now have a much clearer understanding of why a state machine, SysTick, timestamps, `volatile`, and non-blocking scheduling can work together to build a responsive embedded system.

**This is the part of the project I wanted to understand—not just how to write the code, but why the code is structured the way it is.**
