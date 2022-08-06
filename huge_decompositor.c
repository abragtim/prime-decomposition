/* ----------------------------------------------------------------------------
 * BAB36PRGA - Programování v C
 * HW2B - Prvočíselný rozklad
 * Uploaded at 20.5.2022 23:15
 * Score: 5/5
 * ------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#define MAX_VALUE 1000000  // prime numbers belong to the <2; MAX_VALUE>
#define MAX_FIGURE_NUM 100 // number of figures in a huge num
#define NO_FIGURES -999
#define STOP_NUMBER 0 // number stops reading from stdin
#define LOWER 1
#define HIGHER -1
#define EQ 0
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

/* -------------- Eratosthenes Sieve to detect prime numbers --------------- */
long *eratosthenes(long max_value, int *amount_pointer); // Sieve
void check_args_sieve(long max_value, int *amount_pointer);
/* check success of _alloc(): */
void check_alloc(long *temporary_pointer, // pointer for checking
                 long *main_pointer);     // pointer for using in a program
/* -------------- Decompose number into certain prime numbers -------------- */
int decomposition(char *number, int primes_amount, long primes[primes_amount]);
/* check ability of decomposing number during cykle: */
bool check_decomposing_terms(int prime_index, // index in array
                             long prime_number,
                             int primes_amount,
                             char *decomposing_number, //
                             long *primes_array,
                             char *_number);
/* ------------------------------------------------------------------------- */
int scanning_analysis(int scan_status); // scan success analysis
void super_free(long *pointer);         // modified free() for allocated memory
/* -------------- huge numbers operations ---------------------------------- */
int read_huge_number(char *huge_p_num);
// division hn. returns modulo
int hn_divison(char *dividend, long divisor, char *quotient);
long hn_to_long(char *hn, int from, int to); // from hn to long
char figure_to_char(long figure);
int hn_get_quantity(char *_num, long prime, long *primes);
void hn_decrease_number(char *num, long prime, long in_quantity);
void hn_copy(char *from, char *to, int len); // copy hn from to
int hn_is_lower_long(char *hn, long l);      // hn is lower, the long num
bool is_a_hn(char *num);                     // is a huge number [t/f]

int main()
{
    int scan_status; // returned value from read_number() function

    int primes_amount = 0; // global primes counter
    int *amount_pointer;   // pointers to global primes counter
    amount_pointer = &primes_amount;

    long *primes = eratosthenes(MAX_VALUE, amount_pointer); // prime numbers
    char *num = (char *)malloc(sizeof(char) * MAX_FIGURE_NUM); // number for decomposition
    do
    {
        for (int i = 0; i < MAX_FIGURE_NUM; ++i)
        {
            num[i] = '\0';
        }
        scan_status = read_huge_number(num);
        if (!is_a_hn(num))
        {
            break;
        }
        if (scan_status != SCANNED)
        {
            break; // stop scanning cykle from stdin
        }
        for (int i = 0; i < MAX_FIGURE_NUM; ++i)
        {
            if (num[i] == '\n')
            {
                num[i] = '\0';
                break;
            }
        }
        printf("Prvociselny rozklad cisla %s je:\n", num);
        decomposition(num, primes_amount, primes);
    } while (hn_to_long(num, 0, 1) > 0);
    super_free(primes); // free memory of prime numbers
    free(num);
    num = NULL;
    int ret = scanning_analysis(scan_status); // return value

    return ret;
}

int read_huge_number(char *huge_num)
{
    int ret = SCANNED;
    char *rv;
    rv = fgets(huge_num, MAX_FIGURE_NUM + 1, stdin);
    if (rv == NULL) // scanning
    {
        if (ferror(stdin) != 0)
        {
            fprintf(stderr, "ERROR: gets() failed.");
            ret = FAILED_SCANNNG;
            return ret;
        }
        else if ((feof(stdin) != 0))
        {
            ret = STOP_SCANNING;
            return ret;
        }
        else
        {
            fprintf(stderr, "ERROR: Unknown mistake read_huge_number().");
            ret = FAILED_SCANNNG;
            return ret;
        }
        ret = SCANNED;
    }
    if (huge_num[0] == '0')
    {
        return STOP_SCANNING;
    }
    return ret;
}

bool is_a_hn(char *hn)
{
    bool rv = true;
    for (int i = 0; i <= MAX_FIGURE_NUM; ++i)
    {
        switch (hn[i])
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return true;
            break;
        case '\n':
            hn[i] = '\0';
        case '\0':
            break;
        default:
            rv = false;
            return rv;
            break;
        }
    }
    return false;
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

int decomposition(char *_number, int amount, long primes[amount])
{
    char *number = (char *)malloc(sizeof(char) * MAX_FIGURE_NUM);
    // char number[MAX_FIGURE_NUM];
    strcpy(number, _number);
    // hn_copy(_number, number, MAX_FIGURE_NUM);
    if (hn_to_long(number, 0, 1) < 1)
    {
        fprintf(stderr, "ERROR: can't decompose number < 1\n");
        super_free(primes);
        free(number);
        free(_number);
        exit(DECOMPOSE_NUMBER_ERROR);
    }
    if ((hn_to_long(number, 0, 1) == 1) && (hn_to_long(number, 1, 2) == NO_FIGURES))
    {
        printf("1\n");
        free(number);
        return DECOMPOSED_INTO_1;
    }
    bool first_print = false; // first printf() wasn't applied
    for (int j = 0; (hn_to_long(number, 0, 1) != 1) ||
                    (hn_to_long(number, 1, 2) != NO_FIGURES);
         ++j)
    {
        long prime = primes[j];
        bool available = check_decomposing_terms(j,
                                                 prime,
                                                 amount,
                                                 _number,
                                                 primes,
                                                 _number);
        if (!available)
        {
            break;
        }
        // quantity of prime in num:
        // int quantity = hn_get_quantity(number, prime, primes);
        int quantity = 0;
        quantity = hn_get_quantity(number, prime, primes);
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

                hn_decrease_number(number, prime, quantity);
            }
            else // prime^quantity
            {
                printf("%li^%i", prime, quantity);
                first_print = true;

                hn_decrease_number(number, prime, quantity);
            }
        }
    }
    putchar('\n');
    free(number);
    return DECOMPOSED_INTO_PRIMES;
}

bool check_decomposing_terms(int index,
                             long prime,
                             int amount,
                             char *number,
                             long *primes,
                             char *_number)
{
    int cond = hn_is_lower_long(number, prime);
    if (cond == LOWER) // useless primes after
    {
        return false;
    }

    if (prime < MIN_PRIME_NUMBER) // prime numbers >= 2
    {
        fprintf(stderr, "ERROR: out of prime numbers interval\n");
        super_free(primes);
        free(_number);
        free(number);
        exit(DECOMPOSE_INTERVAL_ERROR);
    }
    if (index >= amount) // no primes available for decomposing
    {
        fprintf(stderr, "ERROR: can't decompose into primes <= MAX_VAL\n");
        super_free(primes);
        free(_number);
        free(number);
        exit(DECOMPOSE_BIG_NUMBER_ERROR);
    }

    return true;
}

int hn_get_quantity(char *_num, long prime, long *primes)
{
    // char num[MAX_FIGURE_NUM];
    char *num = (char *)malloc(sizeof(char) * MAX_FIGURE_NUM);
    strcpy(num, _num);
    // hn_copy(_num, num, MAX_FIGURE_NUM);
    if (prime < MIN_PRIME_NUMBER)
    {
        fprintf(stderr, "ERROR: can't count prime in number");
        super_free(primes);
        free(num);
        num = NULL;
        free(_num);
        _num = NULL;
        exit(QUANTITY_COUNT_ERROR);
    }
    char *tmp = (char *)malloc(sizeof(char) * MAX_FIGURE_NUM);
    // char tmp[MAX_FIGURE_NUM]; // tmp result
    int counter = 0;
    int mod;
    while ((mod = hn_divison(num, prime, tmp)) == 0)
    {
        ++counter;
        strcpy(num, tmp);
        // hn_copy(tmp, num, MAX_FIGURE_NUM);
        for (int i = 0; i < MAX_FIGURE_NUM; ++i)
        {
            tmp[i] = '\0';
        }
    }
    free(num);
    num = NULL;
    free(tmp);
    tmp = NULL;
    return counter;
}

void hn_decrease_number(char *num, long prime, long in_quantity)
{
    int comp = hn_is_lower_long(num, prime);
    if ((prime >= MIN_PRIME_NUMBER) && ((comp == HIGHER) || (comp == EQ)))
    {
        for (int i = 0; i < in_quantity; ++i)
        {
            char *tmp = (char *)malloc(sizeof(char) * MAX_FIGURE_NUM);
            // char tmp[MAX_FIGURE_NUM];
            hn_divison(num, prime, tmp);
            strcpy(num, tmp);
            // hn_copy(tmp, num, MAX_FIGURE_NUM);
            free(tmp);
        }
    }
    else
    {
        fprintf(stderr, "ERROR: no prime number division\n");
        // no changes for number
    }
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

/* --- huge numbers operations --- */
int hn_divison(char *_dividend, long divisor, char *quotient)
{
    char *dividend = (char *)malloc(sizeof(char) * MAX_FIGURE_NUM);
    // char dividend[MAX_FIGURE_NUM];
    strcpy(dividend, _dividend);
    // hn_copy(_dividend, dividend, MAX_FIGURE_NUM);
    int d_idx = 0; // char* dividend index
    int q_idx = 0; // char* quotient index
    long part = 0; // active part in calculation
    int modulo = 0;
    while (part < divisor) // first part of dividend
    {
        long c;
        c = hn_to_long(dividend, d_idx, d_idx + 1);
        if (c == NO_FIGURES)
        {
            modulo = (part) % divisor;
            quotient[q_idx++] = '0';
            free(dividend);
            return modulo;
        }
        else
        {
            part = 10 * part + c;
            d_idx++;
        }
    }

    bool quit = false;
    while (!quit)
    {
        quotient[q_idx++] = figure_to_char(part / divisor);
        long c;
        c = hn_to_long(dividend, d_idx, d_idx + 1);
        if (c == NO_FIGURES)
        {
            modulo = part % divisor;
            quit = true;
            break;
        }
        else
        {
            part = 10 * (part % divisor) + c;
            d_idx++;
        }
    }
    for (int i = q_idx; i < MAX_FIGURE_NUM; ++i)
    {
        quotient[i] = '\0';
    }
    free(dividend);
    return modulo;
}

long hn_to_long(char *hn, int f, int t)
{
    long l = 0;
    for (int i = f; i < t; ++i)
    {
        int c_i;
        char c = hn[i];
        switch (c)
        {
        case '0':
            c_i = 0;
            break;
        case '1':
            c_i = 1;
            break;
        case '2':
            c_i = 2;
            break;
        case '3':
            c_i = 3;
            break;
        case '4':
            c_i = 4;
            break;
        case '5':
            c_i = 5;
            break;
        case '6':
            c_i = 6;
            break;
        case '7':
            c_i = 7;
            break;
        case '8':
            c_i = 8;
            break;
        case '9':
            c_i = 9;
            break;
        case '\0':
        case '\n':
            if (l)
            {
                return l;
            }
            return NO_FIGURES;
            break;
        default:
            fprintf(stderr, "ERROR: Can't to long.");
            return NO_FIGURES;
            break;
        }
        l = l * 10 + c_i;
    }
    return l;
}

char figure_to_char(long figure)
{
    switch (figure)
    {
    case 0:
        return '0';
        break;
    case 1:
        return '1';
        break;
    case 2:
        return '2';
        break;
    case 3:
        return '3';
        break;
    case 4:
        return '4';
        break;
    case 5:
        return '5';
        break;
    case 6:
        return '6';
        break;
    case 7:
        return '7';
        break;
    case 8:
        return '8';
        break;
    case 9:
        return '9';
        break;
    default:
        fprintf(stderr, "ERROR: Pretyping not figure to char.\n");
        break;
    }

    return '\0';
}

void string_copy(char *from, char *to, int len)
{
    for (int i = 0; i < len; ++i)
    {
        to[i] = from[i];
    }
}

void hn_copy(char *from, char *to, int len)
{
    for (int i = 0; i < len; ++i)
    {
        switch (from[i])
        {
        case '0':
            to[i] = '0';
            break;
        case '1':
            to[i] = '1';
            break;
        case '2':
            to[i] = '2';
            break;
        case '3':
            to[i] = '3';
            break;
        case '4':
            to[i] = '4';
            break;
        case '5':
            to[i] = '5';
            break;
        case '6':
            to[i] = '6';
            break;
        case '7':
            to[i] = '7';
            break;
        case '8':
            to[i] = '8';
            break;
        case '9':
            to[i] = '9';
            break;
        default:
            to[i] = '\0';
            break;
        }
    }
}

int hn_is_lower_long(char *hn, long l)
{
    bool q = false;
    int counter = 0;
    while (!q)
    {
        int del = pow(10, counter);
        if (l / del >= 1)
        {
            counter++;
            continue;
        }
        q = true;
    }
    long part = hn_to_long(hn, 0, counter + 1);
    if (part < l)
    {
        return LOWER;
    }
    else if (part == l)
    {
        return EQ;
    }
    else
    {
        return HIGHER;
    }

    return LOWER;
}
