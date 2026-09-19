#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "rpi_gpio.h"

/* =========================================================
   SMART STREET LIGHTING WITH PREDICTIVE CONTROL
   QNX 8.0 + Raspberry Pi 4

   LDR  -> GPIO26

   IR1  -> GPIO5
   IR2  -> GPIO6
   IR3  -> GPIO13

   LED1 -> GPIO23
   LED2 -> GPIO24
   LED3 -> GPIO25
   ========================================================= */


/* ================= GPIO PINS ================= */

#define LDR_PIN     26

#define IR1_PIN     5
#define IR2_PIN     6
#define IR3_PIN     13

#define LED1_PIN    23
#define LED2_PIN    24
#define LED3_PIN    25


/* =========================================================
   SENSOR LOGIC

   QNX library:
   GPIO_LOW  = 4
   GPIO_HIGH = 8

   IR sensor:
   Object    -> LOW
   No object -> HIGH
   ========================================================= */

#define IR_DETECTED GPIO_LOW

/*
   LDR:
   Currently using LOW = DAY.

   If your DAY/NIGHT operation is reversed,
   change GPIO_LOW below to GPIO_HIGH.
*/

#define LDR_DAY GPIO_LOW


/* ================= GLOBAL ================= */

volatile sig_atomic_t running = 1;


/* =========================================================
   CTRL+C HANDLER
   ========================================================= */

void stop_program(int sig)
{
    (void)sig;
    running = 0;
}


/* =========================================================
   READ GPIO SENSOR
   ========================================================= */

unsigned read_sensor(int pin)
{
    unsigned level = GPIO_HIGH;

    int result = rpi_gpio_input(pin, &level);

    if (result != GPIO_SUCCESS)
    {
        printf("ERROR reading GPIO%d\n", pin);
    }

    return level;
}


/* =========================================================
   STABLE LED CONTROL

   IMPORTANT:
   GPIO is written ONLY when LED state actually changes.

   There is NO PWM.
   There is NO brightness control.
   LEDs are only ON or OFF.
   ========================================================= */

void set_lights(int z1, int z2, int z3)
{
    static int old_z1 = -1;
    static int old_z2 = -1;
    static int old_z3 = -1;


    /* ---------- LED 1 ---------- */

    if (z1 != old_z1)
    {
        if (z1)
        {
            rpi_gpio_output(LED1_PIN, GPIO_HIGH);
        }
        else
        {
            rpi_gpio_output(LED1_PIN, GPIO_LOW);
        }

        old_z1 = z1;
    }


    /* ---------- LED 2 ---------- */

    if (z2 != old_z2)
    {
        if (z2)
        {
            rpi_gpio_output(LED2_PIN, GPIO_HIGH);
        }
        else
        {
            rpi_gpio_output(LED2_PIN, GPIO_LOW);
        }

        old_z2 = z2;
    }


    /* ---------- LED 3 ---------- */

    if (z3 != old_z3)
    {
        if (z3)
        {
            rpi_gpio_output(LED3_PIN, GPIO_HIGH);
        }
        else
        {
            rpi_gpio_output(LED3_PIN, GPIO_LOW);
        }

        old_z3 = z3;
    }
}


/* =========================================================
   MAIN PROGRAM
   ========================================================= */

int main(void)
{
    unsigned ldr;

    unsigned ir1;
    unsigned ir2;
    unsigned ir3;


    /*
       zone = confirmed zone

       0 = no vehicle
       1 = vehicle in Zone 1
       2 = vehicle in Zone 2
       3 = vehicle in Zone 3
    */

    int zone = 0;


    /*
       candidate_zone = current sensor reading that
       is waiting to be confirmed.
    */

    int candidate_zone = 0;


    /*
       Number of consecutive identical readings.
    */

    int stable_count = 0;


    signal(SIGINT, stop_program);


    printf("\n");
    printf("============================================\n");
    printf(" SMART STREET LIGHTING\n");
    printf(" WITH PREDICTIVE CONTROL\n");
    printf(" QNX 8.0 - RASPBERRY PI 4\n");
    printf("============================================\n\n");


    /* =====================================================
       SENSOR GPIO SETUP
       ===================================================== */


    /* ---------- LDR ---------- */

    if (rpi_gpio_setup(LDR_PIN, GPIO_IN) != GPIO_SUCCESS)
    {
        printf("ERROR: LDR setup failed\n");

        return EXIT_FAILURE;
    }


    /* ---------- IR 1 ---------- */

    if (rpi_gpio_setup(IR1_PIN, GPIO_IN) != GPIO_SUCCESS)
    {
        printf("ERROR: IR1 setup failed\n");

        return EXIT_FAILURE;
    }


    /* ---------- IR 2 ---------- */

    if (rpi_gpio_setup(IR2_PIN, GPIO_IN) != GPIO_SUCCESS)
    {
        printf("ERROR: IR2 setup failed\n");

        return EXIT_FAILURE;
    }


    /* ---------- IR 3 ---------- */

    if (rpi_gpio_setup(IR3_PIN, GPIO_IN) != GPIO_SUCCESS)
    {
        printf("ERROR: IR3 setup failed\n");

        return EXIT_FAILURE;
    }


    /* =====================================================
       LED GPIO SETUP
       ===================================================== */


    if (rpi_gpio_setup(LED1_PIN, GPIO_OUT) != GPIO_SUCCESS)
    {
        printf("ERROR: LED1 setup failed\n");

        return EXIT_FAILURE;
    }


    if (rpi_gpio_setup(LED2_PIN, GPIO_OUT) != GPIO_SUCCESS)
    {
        printf("ERROR: LED2 setup failed\n");

        return EXIT_FAILURE;
    }


    if (rpi_gpio_setup(LED3_PIN, GPIO_OUT) != GPIO_SUCCESS)
    {
        printf("ERROR: LED3 setup failed\n");

        return EXIT_FAILURE;
    }


    /* =====================================================
       INITIAL STATE

       All lights OFF.
       ===================================================== */

    set_lights(0, 0, 0);


    printf("GPIO INITIALIZATION SUCCESSFUL\n");
    printf("IR SENSOR FILTER ENABLED\n");
    printf("SYSTEM STARTED\n\n");


    /* =====================================================
       MAIN CONTROL LOOP
       ===================================================== */

    while (running)
    {
        int detected_zone = 0;


        /* =================================================
           READ REAL SENSOR VALUES
           ================================================= */

        ldr = read_sensor(LDR_PIN);

        ir1 = read_sensor(IR1_PIN);
        ir2 = read_sensor(IR2_PIN);
        ir3 = read_sensor(IR3_PIN);


        /* =================================================
           DISPLAY RAW VALUES

           LOW  = 4
           HIGH = 8
           ================================================= */

        printf(
            "RAW | LDR=%u | IR1=%u IR2=%u IR3=%u | ",
            ldr,
            ir1,
            ir2,
            ir3
        );


        /* =================================================
           DAY MODE

           During daytime all street lights remain OFF.
           ================================================= */

        if (ldr == LDR_DAY)
        {
            zone = 0;

            candidate_zone = 0;

            stable_count = 0;


            set_lights(0, 0, 0);


            printf(
                "DAY | "
                "LED1=OFF LED2=OFF LED3=OFF\n"
            );
        }


        /* =================================================
           NIGHT MODE
           ================================================= */

        else
        {

            /* =============================================
               CHECK IR SENSORS

               IR is ACTIVE LOW.

               GPIO_LOW means vehicle/object detected.
               ============================================= */


            /* ---------- ZONE 1 ---------- */

            if (ir1 == IR_DETECTED)
            {
                detected_zone = 1;
            }


            /* ---------- ZONE 2 ---------- */

            else if (ir2 == IR_DETECTED)
            {
                detected_zone = 2;
            }


            /* ---------- ZONE 3 ---------- */

            else if (ir3 == IR_DETECTED)
            {
                detected_zone = 3;
            }


            /* ---------- NO VEHICLE ---------- */

            else
            {
                detected_zone = 0;
            }


            /* =================================================
               SENSOR STABILITY FILTER

               IR sensors can sometimes fluctuate:

               8 -> 4 -> 8 -> 4

               We DO NOT immediately change the LEDs.

               The same condition must appear THREE times
               consecutively before the lighting state changes.

               Main loop = 100 ms

               3 readings = approximately 300 ms
               ================================================= */


            if (detected_zone == candidate_zone)
            {
                stable_count++;
            }

            else
            {
                candidate_zone = detected_zone;

                stable_count = 1;
            }


            /* ---------- Confirm stable detection ---------- */

            if (stable_count >= 3)
            {
                zone = candidate_zone;

                /*
                   Prevent counter from increasing forever.
                */

                stable_count = 3;
            }


            /* =================================================
               PREDICTIVE STREET LIGHT CONTROL
               ================================================= */


            /* =============================================
               VEHICLE IN ZONE 1

               Zone 1 = ON

               Zone 2 = ON BEFORE vehicle reaches it.

               This is predictive lighting.
               ============================================= */

            if (zone == 1)
            {
                set_lights(1, 1, 0);


                printf(
                    "NIGHT | ZONE 1 DETECTED | "
                    "LED1=ON | "
                    "LED2=ON (PREDICTED) | "
                    "LED3=OFF\n"
                );
            }


            /* =============================================
               VEHICLE IN ZONE 2

               Zone 2 = ON

               Zone 3 = ON BEFORE vehicle reaches it.
               ============================================= */

            else if (zone == 2)
            {
                set_lights(0, 1, 1);


                printf(
                    "NIGHT | ZONE 2 DETECTED | "
                    "LED1=OFF | "
                    "LED2=ON | "
                    "LED3=ON (PREDICTED)\n"
                );
            }


            /* =============================================
               VEHICLE IN ZONE 3
               ============================================= */

            else if (zone == 3)
            {
                set_lights(0, 0, 1);


                printf(
                    "NIGHT | ZONE 3 DETECTED | "
                    "LED1=OFF | "
                    "LED2=OFF | "
                    "LED3=ON\n"
                );
            }


            /* =============================================
               NO VEHICLE

               Energy saving mode.
               ============================================= */

            else
            {
                set_lights(0, 0, 0);


                printf(
                    "NIGHT | NO VEHICLE | "
                    "LED1=OFF LED2=OFF LED3=OFF\n"
                );
            }
        }


        /*
           Force console output immediately.
        */

        fflush(stdout);


        /*
           Read sensors every 100 milliseconds.
        */

        usleep(100000);
    }


    /* =====================================================
       SAFE SHUTDOWN
       ===================================================== */

    printf("\n");
    printf("Stopping Smart Street Lighting System...\n");


    /* Turn all LEDs OFF */

    set_lights(0, 0, 0);


    /* Release GPIO resources */

    rpi_gpio_cleanup();


    printf("GPIO cleanup completed.\n");
    printf("System stopped safely.\n");


    return EXIT_SUCCESS;
}
