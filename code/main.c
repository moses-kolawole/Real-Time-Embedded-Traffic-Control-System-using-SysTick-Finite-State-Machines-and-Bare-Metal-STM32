#include "main.h"

/*=====================================================
                Global Variables
=====================================================*/

// Incremented every 1 ms inside SysTick interrupt

volatile uint32_t systemTime = 0;

// Stores the time a state begins
volatile uint32_t stateStartTime = 0;

/*=====================================================
                Traffic States
=====================================================*/

typedef enum
{
    STATE_A_GREEN,
    STATE_A_YELLOW,
    ALL_RED_1,
    STATE_B_GREEN,
    STATE_B_YELLOW,
    ALL_RED_2

}TrafficState;

// Initial traffic state
TrafficState currentState = STATE_A_GREEN;



/*=====================================================
                    Main Program
=====================================================*/

int main(void)
{

    /*-----------------------------------------------
            Initialize GPIO Pins
    -----------------------------------------------*/

    RCC->AHB1ENR |= (1 << 0);   // GPIOA clock
    RCC->AHB1ENR |= (1 << 1);   // GPIOB clock

    // First Red LED
    GPIOB->MODER &= ~(3 << (0 * 2));
    GPIOB->MODER |=  (1 << (0 * 2));

    // First Yellow LED
    GPIOB->MODER &= ~(3 << (1 * 2));
    GPIOB->MODER |=  (1 << (1 * 2));

    // First Green LED
    GPIOB->MODER &= ~(3 << (2 * 2));
    GPIOB->MODER |=  (1 << (2 * 2));

    // Second Red LED
    GPIOA->MODER &= ~(3 << (3 * 2));
    GPIOA->MODER |=  (1 << (3 * 2));

    // Second Yellow LED
    GPIOA->MODER &= ~(3 << (4 * 2));
    GPIOA->MODER |=  (1 << (4 * 2));

    // Second Green LED
    GPIOA->MODER &= ~(3 << (5 * 2));
    GPIOA->MODER |=  (1 << (5 * 2));


    /*-----------------------------------------------
            Initialize SysTick (1 ms)
    -----------------------------------------------*/

    SysTick_Config(16000);


    /*-----------------------------------------------
            Set Initial State
    -----------------------------------------------*/

    currentState = STATE_A_GREEN;

    stateStartTime = systemTime;


    /*-----------------------------------------------
            Enter while(1)
    -----------------------------------------------*/

    while(1)
    {

        /*===========================================
                STATE_A_GREEN ?
        ===========================================*/

        if(currentState == STATE_A_GREEN)
        {

            /* Road A Green */
            GPIOB->ODR &= ~(1 << 0);
            GPIOB->ODR &= ~(1 << 1);
            GPIOB->ODR |=  (1 << 2);

            /* Road B Red */
            GPIOA->ODR |=  (1 << 3);
            GPIOA->ODR &= ~(1 << 4);
            GPIOA->ODR &= ~(1 << 5);

            if((systemTime - stateStartTime) >= 5000)
            {
                currentState = STATE_A_YELLOW;
                stateStartTime = systemTime;
            }

        }


        /*===========================================
                STATE_A_YELLOW ?
        ===========================================*/

        else if(currentState == STATE_A_YELLOW)
        {

            GPIOB->ODR &= ~(1 << 0);
            GPIOB->ODR |=  (1 << 1);
            GPIOB->ODR &= ~(1 << 2);

            GPIOA->ODR |=  (1 << 3);
            GPIOA->ODR &= ~(1 << 4);
            GPIOA->ODR &= ~(1 << 5);

            if((systemTime - stateStartTime) >= 2000)
            {
                currentState = ALL_RED_1;
                stateStartTime = systemTime;
            }

        }


        /*===========================================
                ALL_RED_1 ?
        ===========================================*/

        else if(currentState == ALL_RED_1)
        {

            GPIOB->ODR |= (1 << 0);
            GPIOB->ODR &= ~(1 << 1);
            GPIOB->ODR &= ~(1 << 2);

            GPIOA->ODR |= (1 << 3);
            GPIOA->ODR &= ~(1 << 4);
            GPIOA->ODR &= ~(1 << 5);

            if((systemTime - stateStartTime) >= 1000)
            {
                currentState = STATE_B_GREEN;
                stateStartTime = systemTime;
            }

        }


        /*===========================================
                STATE_B_GREEN ?
        ===========================================*/

        else if(currentState == STATE_B_GREEN)
        {

            GPIOB->ODR |= (1 << 0);
            GPIOB->ODR &= ~(1 << 1);
            GPIOB->ODR &= ~(1 << 2);

            GPIOA->ODR &= ~(1 << 3);
            GPIOA->ODR &= ~(1 << 4);
            GPIOA->ODR |=  (1 << 5);

            if((systemTime - stateStartTime) >= 5000)
            {
                currentState = STATE_B_YELLOW;
                stateStartTime = systemTime;
            }

        }


        /*===========================================
                STATE_B_YELLOW ?
        ===========================================*/

        else if(currentState == STATE_B_YELLOW)
        {

            GPIOB->ODR |= (1 << 0);
            GPIOB->ODR &= ~(1 << 1);
            GPIOB->ODR &= ~(1 << 2);

            GPIOA->ODR &= ~(1 << 3);
            GPIOA->ODR |=  (1 << 4);
            GPIOA->ODR &= ~(1 << 5);

            if((systemTime - stateStartTime) >= 2000)
            {
                currentState = ALL_RED_2;
                stateStartTime = systemTime;
            }

        }


        /*===========================================
                ALL_RED_2 ?
        ===========================================*/

        else if(currentState == ALL_RED_2)
        {

            GPIOB->ODR |= (1 << 0);
            GPIOB->ODR &= ~(1 << 1);
            GPIOB->ODR &= ~(1 << 2);

            GPIOA->ODR |= (1 << 3);
            GPIOA->ODR &= ~(1 << 4);
            GPIOA->ODR &= ~(1 << 5);

            if((systemTime - stateStartTime) >= 1000)
            {
                currentState = STATE_A_GREEN;
                stateStartTime = systemTime;
            }

        }

    }

}
