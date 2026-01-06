# eCjp
## Easy C JSON Parser

Questa è una semplice libreria scritta interamente in C che implementa un parser di una stringa di caratteri UTF-8 che contiene una struttura JSON.  
Ci sono molte altre librerie scritte in C già pronte; ho voluto scrivere questa per usarla nei miei progetti senza dovermi preoccupare delle condizioni d'uso delle altre.  
Le caratteristiche principali della libreria sono:
- non dipende da altre librerie, dipende solo dalla libc standard;
- non usa la ricorsione (se necessaria deve essere implementata dal programma principale);
- può essere compilata per MCU o CPU in funzione di quante risorse (memoria e velocità) sono disponibili;
- non modifica il JSON originale;
- esegue il parser di una struttura JSON indicando dove si trova il primo errore di sintassi;
- tollera chiavi duplicate;
- è libera secondo quanto prevede la licenza BSD 3-Clause;
- un solo file sorgente e due file include (oltre al config.h generato dal *configure*)  

La libreria realizza **solo il parser di un JSON**, non la sua creazione o modifica.  

Nel codice sorgente ci sono due implementazioni:  
- Una implementazione scompone in token il primo livello di una struttura JSON creando una lista di token.  
- Una implementazione alternativa crea una lista con la posizione e lunghezza di tutte le chiavi all'interno del JSON.  

In entrambi i casi, la memoria allocata per le liste deve essere liberata dal programma chiamante dopo l'utilizzo: la libreria mette a disposizione delle funzioni apposite per farlo.  

----------

1. [Prerequisite](#prerequisite)
2. [Project tree](#project-tree)
3. [Build on Linux](#build-on-linux)
4. [API](#api)
5. [Examples](#examples)
6. [Tests](#tests)
7. [Remarks](#remarks)

## Prerequisite
Per compilare la libreria su Linux sono necessari i seguenti tools:
- **gcc** (tested with gcc 14.2.0)
- **make** (tested with GNU Make 4.4.1)
- **autoconf** (tested with GNU Autoconf 2.72)

## Project tree
Una volta scaricato il progetto le cartelle principali sono:
```
build
include
src
scripts
tests
```
| Directory                     | Description                       |
|-------------------------------|-----------------------------------|
| build                         | build directory                   |
| include                       | include directory                 |
| src                           | source directory: contains the library source and all the tests programs    |
| scripts                       | contains the scripts to run the tests |
| tests                         | contains many files with valid and invalid JSON structures used to perfom tests|

I files della libreria sono solo i seguenti: *ecjp.c, ecjp.h, ecjp_limit.h* più l'include *config.h* generato dal tool *configure*.

## Build on Linux

Dopo aver scaricato il sorgente, invocare i seguenti comandi:

``` sh
$ autoreconf -i
$ cd build
```

Le opzioni di compilazione disponibili possono essere visualizzate invocando il comando:

``` sh
$ ../configure -h
```

quelle proprie della libreria sono:

``` sh
--enable-debug          Enable DEBUG macro
--enable-debug-verbose  Enable DEBUG_VERBOSE macro
--enable-token-list     Enable ECJP_TOKEN_LIST macro
--enable-run-on-pc      Enable ECJP_RUN_ON_PC macro
--enable-run-on-mcu     Enable ECJP_RUN_ON_MCU macro
```
L'effetto di ciascuna di queste opzioni è descritto nella tabella seguente:

| OPTION                    |   DESCRIPTION                             |
|---------------------------|-------------------------------------------|
| --enable-debug            | enable DEBUG print on stdout              |
| --enable-debug-verbose    | enable VERBOSE print on stdout            |
| --enable-token-list       | compile token-list version of the library |
| --enable-run-on-pc        | set the limits in the code to run on CPU  |
| --enable-run-on-mcu       | set the limits in the code to run on MCU  |

Abilitando l'opzione *run-on-mcu* la libreria utilizza poca memoria ma imposta dei limiti molto bassi sulla lunghezza delle strutture che può analizzare o su quanti livelli può avere la struttura JSON.  
Inoltre, con questa opzione, le opzioni *debug* e *debug-verbose* non hanno effetto perchè **quando compilata per MCU tutti gli output sono soppressi**.  
Invocare il *configure* con le opzioni desiderate.  
Ad esempio per compilare per PC l'implementazione con la scomposizione a token:  
``` sh
$ ../configure --enable-debug --enable-token-list --enable-run-on-pc
```  
Per compilare, sempre dalla cartella *build*, invocare il comando:  
``` sh
$ make
```  
la compilazione produce:  
- nella cartella `build/src/.libs` la libreria **libecjp.so.0.0.0** e **libecjp.a**.
- nella cartella `build/src/.libs` gli eseguibili seguenti:
    -   example_ecjp_X = programma di esempio di utilizzo della libreria
    -   test_lib_X = programmi di esempio per testare funzioni specifiche della libreria

Alcuni programmi di esempio e di test funzionano in base al tipo di compilazione: se l'opzione *token-list* non è supportata il programma esce con un messaggio di errore.

## API
La libreria mette a disposizione una serie di API che insieme permettono il parser di JSON di complessità anche elevata.  
**Per il momento questa sezione descrive le funzioni dell'implementazione abilitata dall'opzione *token-list* che scompone il JSON in token.**

### ecjp_check_syntax_2()
La funzione **ecjp_check_syntax_2()** permette di verificare la correttezza sintattica di una struttura JSON passata come argomento.  
Quando compilata per CPU, la funzione restituisce in output la posizione nella struttura JSON del primo errore, ad esempio:  
``` sh
Testing input file: ../../tests/invalid_15_missing_quote_in_key.json

Testing JSON file (../../tests/invalid_15_missing_quote_in_key.json) of size 35 bytes:
ecjp_check_and_load_2 - 938: Expected colon after key, received: V
ecjp_check_syntax_2() on JSON file: FAILED with error code: 3
ecjp_check_syntax_2(): Error position: 27
ecjp_show_error - 350: Error at position 27 (row 1, column 28):
{ "KEY1": "VAL1", "KEY_2: "VAL2" } 
---------------------------^

```  
`ecjp_return_code_t ecjp_check_syntax_2(const char *input, ecjp_check_result_t *res)`

This function call ecjp_check_and_load_2() without pointer to store the items to perform only syntax checking.  

Parameters:
- input: The JSON-like input string to be checked.
- res: Pointer to a structure to store the result of the check, including any error position.  

Returns:
- ECJP_NO_ERROR if the input string is a JSON valid structure.
- ECJP_NULL_POINTER if any input pointer is NULL.
- ECJP_EMPTY_STRING if the input string is empty.
- ECJP_SYNTAX_ERROR if there is a syntax error in the input string.  

La struttura puntata da *res* contiene i seguenti campi:
```
typedef struct ecjp_check_result {
    int                 err_pos;
    ECJP_TYPE_POS_KEY   num_keys;
    ecjp_struct_type_t  struct_type;
    int                 memory_used;
} ecjp_check_result_t;
```
e la funzione torna il tipo di struttura JSON analizzata nel campo *struct_type*: ECJP_ST_OBJ per un oggetto, ECJP_ST_ARRAY per un array.  

### ecjp_load_2()
`ecjp_return_code_t ecjp_load_2(const char *input, ecjp_item_elem_t **item_list, ecjp_check_result_t *res)`

This function call ecjp_check_and_load_2() with pointer to store the items. Perform syntax checking and store all items found in the input string at the first level of the structure.  

Parameters:
- input: The JSON-like input string to be checked and loaded.
- item_list: Pointer to a list of item elements loaded with the items found in the input string.
- res: Pointer to a structure to store the result of the check, including any error position.  

Returns:
- ECJP_NO_ERROR if the input string is valid.
- ECJP_NULL_POINTER if any input pointer is NULL.
- ECJP_EMPTY_STRING if the input string is empty.
- ECJP_SYNTAX_ERROR if there is a syntax error in the input string.

La funzione alloca tanti *ecjp_item_elem_t* per quanti elementi trova al primo livello della struttura JSON.  
Se il JSON è un oggetto, gli item sono le coppie chiave-valore, se il JSON è un array gli item sono gli elementi dell'array e per ciascuno di essi viene specificato il suo tipo.  
**Ogni item, sia che si tratti di una coppia chiave-valore, sia che si tratti di un elemento di un array, viene salvato come stringa**.  
La funzione chiamante, quando non ne ha più bisogno, deve liberare la memoria allocata chiamando la funzione *ecjp_free_item_list()*.  
Nella struttura puntata da *res* c'è il campo *memory_used* che contiene il totale in bytes della memoria allocata nel parsing.  

### ecjp_check_and_load_2()
`ecjp_return_code_t ecjp_check_and_load_2(const char *input, ecjp_item_elem_t **item_list, ecjp_check_result_t *res)`  

This function checks the syntax of a JSON-like input string and loads item tokens into a linked list if the syntax is valid.  

Parameters:
- input: The JSON-like input string to be checked and loaded.
- item_list: Pointer to a list of item elements loaded with the item tokens found in the input string.
- res: Pointer to a structure to store the result of the check, including any error position.  

Returns:
- ECJP_NO_ERROR if the input string is valid.
- ECJP_NULL_POINTER if any input pointer is NULL.
- ECJP_EMPTY_STRING if the input string is empty.
- ECJP_SYNTAX_ERROR if there is a syntax error in the input string.

Se la struttura JSON passata in *input* ha delle chiavi duplicate (anche se non è una buona pratica), la funzione *ecjp_load_2()* o *ecjp_check_and_load_2()* le carica ugualmente tutte in item diversi. 

### ecjp_free_item_list()
`ecjp_return_code_t ecjp_free_item_list(ecjp_item_elem_t **item_list)`  

This function free the memory allocated for a list of item elements.  

Parameters:
- item_list: Pointer to the list of item elements to be freed.  

Returns:
- ECJP_NO_ERROR if the list is freed successfully.

### ecjp_read_element()
`ecjp_return_code_t ecjp_read_element(ecjp_item_elem_t *item_list, int index, ecjp_outdata_t *out)`  

This function reads an element from the item list by its index and copies its value to the output structure.  

Parameters:
- item_list: Pointer to the head of the ecjp_item_elem_t linked list.
- index: The index of the element to read.
- out: Pointer to an ecjp_outdata_t structure where the output will be stored.  

Returns:
- ECJP_NO_ERROR on success
- ECJP_NULL_POINTER if item_list or out is NULL
- ECJP_INDEX_OUT_OF_BOUNDS if the index is out of bounds

La struttura puntata da *out* contiene i seguenti campi:  
```
typedef struct ecjp_outdata {
    ecjp_return_code_t  error_code;
    ECJP_TYPE_POS_KEY   last_pos;
    ECJP_TYPE_LEN_KEY   length;
    ecjp_value_type_t   type;
    void                *value;
    unsigned int        value_size;
} ecjp_outdata_t;
```  
Il campo *value* al momento è volutamente un puntatore a *void* ma la funzione copia la stringa che contiene l'item.  
Se la struttura JSON originale era un oggetto, dentro value sarà memorizzata una coppia chiave-valore, se era un array, sarà memorizzato l'elemento dell'array di indice *index*.


### ecjp_split_key_and_value()
`ecjp_return_code_t ecjp_split_key_and_value(ecjp_item_elem_t *item_list, char *key, char *value, ecjp_bool_t leave_quotes)`  

This function splits a key-value pair item into separate key and value strings.  

Parameters:
- item_list: Pointer to the ecjp_item_elem_t containing the key-value pair.
- key: Pointer to a char array where the extracted key will be stored.
- value: Pointer to a char array where the extracted value will be stored.
- leave_quotes: Boolean flag indicating whether to retain quotes around the key.  

Returns:
- ECJP_NO_ERROR on success
- ECJP_NULL_POINTER if any input pointer is NULL
- ECJP_SYNTAX_ERROR if the item is not a key-value pair
- ECJP_NO_SPACE_IN_BUFFER_VALUE if the key or value exceeds maximum length of the buffers  

Questa funzione permette di separare una coppia chiave-valore in due strighe separate.  
Se l'item non contiene una coppia chiave-valore viene restituito un errore.  

### ecjp_read_key_2()
`ecjp_return_code_t ecjp_read_key_2(ecjp_item_elem_t *item_list, const char *key, unsigned int index, ecjp_outdata_t *out)`  

This function reads the value associated with a specified key from the item list, starting from a given index.  

Parameters:
- item_list: Pointer to the head of the ecjp_item_elem_t linked list.
- key: The key to search for.
- index: The starting index for the search.
- out: Pointer to an ecjp_outdata_t structure where the output will be stored.  

Returns:
- ECJP_NO_ERROR on success
- ECJP_NULL_POINTER if item_list, key, or out is NULL
- ECJP_INDEX_NOT_FOUND if the key is not found in the list

Questa funzione cerca la chiave *key* dentro la lista di item partendo dall'indice specificato da *index* e controllando solo gli item di tipo chiave-valore.  
La funzione torna nella struttura puntata da *out*, nel campo *last_pos*, la posizione nella lista in cui ha trovato la chiave: questo permette di invocare di nuovo la funzione con *index=last_pos* per continuare a scorrere la lista cercando eventuali chiavi duplicate.  
Quando la funzione torna ECJP_INDEX_NOT_FOUND vuol dire che ha completato la lista senza trovare la chiave (o altre chiavi). 

## Examples

## Tests

## Remarks






