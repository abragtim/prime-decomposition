/* ----------------------------------------------------------------------------
 * BAB36PRGA - Programování v C
 * HW2 - Prvočíselný rozklad
 * Uploaded at 24.3.2022 21:14
 * Score: 3/3
 * ------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define MAX_VALUE 1000000 // prime numbers belong to the <2; MAX_VALUE>
#define STOP_NUMBER 0     // number stops reading from stdin
/* ---- SCANNING STATUSES: ---- */
#define SCANNED 0
#define STOP_SCANNING 1
#define FAILED_SCANNNG -1
/* ---- MIN VALUES: ---- */
#define MIN_PRIME_NUMBER 2
#define MIN_EVEN_NUMBER 2
/* ---- DECOMPOSING STATUSES: ---- */
#define DECOMPOSED_INTO_1 1
#define DECOMPOSED_INTO_PRIMES 0
/* ---- ERROR CODES: ---- */
#define INPUT_ERROR 100
#define SIEVE_VALUE_ERROR 200
#define SIEVE_AMOUNT_ERROR 201
#define ALLOC_ERROR 400
#define DECOMPOSE_NUMBER_ERROR 300
#define DECOMPOSE_INTERVAL_ERROR 301
#define DECOMPOSE_BIG_NUMBER_ERROR 302
#define QUANTITY_COUNT_ERROR 500
#define UNDEFINED_ERROR 600

#define bool _Bool

int read_number(long *pointer_num); // read number from stdin
/* -------------- Eratosthenes Sieve to detect prime numbers --------------- */
long *eratosthenes(long max_value, int *amount_pointer); // Sieve
void check_args_sieve(long max_value, int *amount_pointer);
/* check success of _alloc(): */
void check_alloc(long *temporary_pointer, // pointer for checking
                 long *main_pointer);     // pointer for using in a program
/* -------------- Decompose number into certain prime numbers -------------- */
int decomposition(long number, int primes_amount, long primes[primes_amount]);
/* check ability of decomposing number during cykle: */
bool check_decomposing_terms(int prime_index, // index in array
                             int prime_number,
                             int primes_amount,
                             long decomposing_number, //
                             long *primes_array);
/* get amount of prime in number: */
int get_quantity(long number, long prime, long *primes_array);
/* decrease number in certain number of primes: */
long decrease_number(long num, long prime, long in_quantity);
/* ------------------------------------------------------------------------- */
int scanning_analysis(int scan_status); // scan success analysis
void super_free(long *pointer);         // modified free() for allocated memory

int main()
{
    int scan_status; // returned value from read_number() function

    int primes_amount = 0; // global primes counter
    int *amount_pointer;   // pointers to global primes counter
    amount_pointer = &primes_amount;

    long *primes = eratosthenes(MAX_VALUE, amount_pointer); // prime numbers

    long num;    // number for decomposition
    long *p_num; // pointer to num
    p_num = &num;
    do
    {
        scan_status = read_number(p_num);
        if (scan_status != SCANNED)
        {
            break; // stop scanning cykle from stdin
        }
        printf("Prvociselny rozklad cisla %li je:\n", num);
        decomposition(num, primes_amount, primes);
    } while (num > 0);
    super_free(primes); // free memory of prime numbers

    int ret = scanning_analysis(scan_status); // return value

    return ret;
}

int read_number(long *p_num)
{
    int ret; // return status of scanning

    if (1 != scanf("%li", p_num))
    {
        ret = FAILED_SCANNNG;
        return ret;
    }
    /* checking the interval of number: */
    long num = *p_num;
    if (num > 0)
    {
        ret = SCANNED;
    }
    else if (num == STOP_NUMBER)
    {
        ret = STOP_SCANNING;
    }
    else
    {
        ret = FAILED_SCANNNG;
    }

    return ret;
}

long *eratosthenes(long max, int *amount_pointer)
{
    check_args_sieve(max, amount_pointer); // checking arguments
    int counter = 0;                       // local counter of primes
    bool numbers[max];                     // a list of numbers as indxs
    max = max + 1;

    for (long j = 0; j < max; ++j)
    {
        numbers[j] = true; // mark number as potantionally prime
    }

    long *primes = (long *)malloc(sizeof(long) * max); // prime numbers
    check_alloc(primes, primes);
    for (long i = MIN_PRIME_NUMBER; i < max; i = i + 2)
    { // iterate skipping one number:
        if (numbers[i] == true)
        {
            primes[counter++] = i; // add number in primes
            for (long k = i * i; k < max; k = k + i)
            {
                if (numbers[k])
                {
                    numbers[k] = false; // mark as non prime number
                }
            }
        }
        if (i == MIN_EVEN_NUMBER)
        {
            i--; // after: iteration only in odds
        }
    }
    // decrease *primes:
    long *tmp = (long *)realloc(primes, sizeof(long) * counter);
    check_alloc(tmp, primes);
    primes = tmp;

    // fill global counter of primes:
    *amount_pointer = counter;

    return primes;
}

void check_args_sieve(long max, int *amount)
{
    if (max > MAX_VALUE)
    {
        fprintf(stderr, "ERROR: value > MAX_VALUE in preprocessing\n");
        exit(SIEVE_VALUE_ERROR);
    }
    if (amount == NULL)
    {
        fprintf(stderr, "ERROR: amount_pointer not found\n");
        exit(SIEVE_AMOUNT_ERROR);
    }
}

void check_alloc(long *tmp_pointer, long *main_pointer)
{
    if (tmp_pointer == NULL) // if no memory
    {
        if (tmp_pointer != main_pointer) // if realloc
        {
            super_free(main_pointer);
        }
        fprintf(stderr, "ERROR: memory problem.");
        exit(ALLOC_ERROR);
    }
}

int decomposition(long number, int amount, long primes[amount])
{
    if (number < 1)
    {
        fprintf(stderr, "ERROR: can't decompose number < 1\n");
        super_free(primes);
        exit(DECOMPOSE_NUMBER_ERROR);
    }
    if (number == 1)
    {
        printf("1\n");
        return DECOMPOSED_INTO_1;
    }

    bool first_print = false; // first printf() wasn't applied
    for (int j = 0; number != 1; ++j)
    {
        long prime = primes[j];

        bool available = check_decomposing_terms(j,
                                                 prime,
                                                 amount,
                                                 number,
                                                 primes);
        if (!available)
        {
            break;
        }
        // quantity of prime in num:
        int quantity = get_quantity(number, prime, primes);
        if (quantity != 0) // don't print a^0 = 1
        {
            if (first_print == true)
            {
                printf(" x "); // multiplying-symbol between primes
            }

            if (quantity == 1) // prime^1 = prime
            {
                printf("%li", prime);
                first_print = true;

                number = decrease_number(number, prime, quantity);
            }
            else // prime^quantity
            {
                printf("%li^%i", prime, quantity);
                first_print = true;

                number = decrease_number(number, prime, quantity);
            }
        }
    }
    putchar('\n');

    return DECOMPOSED_INTO_PRIMES;
}

bool check_decomposing_terms(int index,
                             int prime,
                             int amount,
                             long number,
                             long *primes)
{
    if (prime > number) // useless primes after
    {
        return false;
    }
    if (prime < MIN_PRIME_NUMBER) // prime numbers >= 2
    {
        fprintf(stderr, "ERROR: out of prime numbers interval\n");
        super_free(primes);
        exit(DECOMPOSE_INTERVAL_ERROR);
    }
    if (index >= amount) // no primes available for decomposing
    {
        fprintf(stderr, "ERROR: can't decompose into primes <= MAX_VAL\n");
        super_free(primes);
        exit(DECOMPOSE_BIG_NUMBER_ERROR);
    }

    return true;
}

int get_quantity(long num, long prime, long *primes)
{
    if (prime < MIN_PRIME_NUMBER || prime > num)
    {
        fprintf(stderr, "ERROR: can't count prime in number");
        super_free(primes);
        exit(QUANTITY_COUNT_ERROR);
    }
    int counter = 0;
    while (num % prime == 0)
    {
        ++counter;
        num = num / prime;
    }

    return counter;
}

long decrease_number(long num, long prime, long in_quantity)
{
    if (prime >= MIN_PRIME_NUMBER && prime <= num)
    {
        num = num / pow(prime, in_quantity);
    }
    else
    {
        fprintf(stderr, "ERROR: no prime number division\n");
        // no changes for number
    }

    return num;
}

int scanning_analysis(int scan_status)
{
    int ret; // return value
    if (scan_status == STOP_SCANNING)
    {
        ret = EXIT_SUCCESS;
    }
    else if (scan_status == FAILED_SCANNNG)
    {
        ret = INPUT_ERROR;
        fprintf(stderr, "Error: Chybny vstup!\n");
    }
    else
    {
        ret = UNDEFINED_ERROR; // possibility of uknown mistake
                               // (ill-designed errors checking)
        fprintf(stderr, "Error: UNKNOWN!\n");
    }

    return ret;
}

void super_free(long *pointer)
{
    if (pointer != NULL)
    {
        free(pointer);
        pointer = NULL;
    }
}
