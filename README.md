## Election system

This program is the implementation of an electoral system. This system follows an electronic voting protocol that guarantees the correct counting of votes and verification of the vote by the voter.
This system uses the RSA algorithm.

1. each voter receives an identifier i and a RSA key pair, e and d
2. each voter votes by encrypting they vote, i and E(i,v)
3. the electoral zone ignores duplicate votes and publishes each vote received
4. each voter sends they private key and they identification to the zone
5. the electoral zone decrypts the votes, printing v and E(i, v) for each vote. Invalid votes are discarded, but are printed
6. each voter verifies that the vote is correct and says if not, by publishing i, E(i, v) and d
7. the electoral zone publishes the final count

The encrypted vote is actually E(f(i,v)), because RSA encrypts one value at a time. The encrypted message m = f(i,v) = 10 x i + v. To decode m, v = m mod 10 and i = floor (m / 10).
The RSA algorithm uses two prime numbers: p and q. This program generates two numbers and tests whether they are prime using the Miller-Rabin algorithm.
The public key is e, that 3 < e < p*q-1, and the private key is d, that d := e^(-1) (mod (p-1) * (q-1))
To encrypt a message m: m^d (mod p*q); to decipher: m^e (mod p*q).


The program has different operations and you can execute it in Voter mode or Zone mode.

For the Voter modality, voters have the following operations:
First, the voter must create the key pairs or use predefined key pairs.
The identification operation creates the keys. This operation receives an identifier i and an integer t. The generated prime numbers are values between t and 2t-1.
Impersonate operation uses predefined keys. This operation receives an identifier i and the keys, p q and d, and stores them.

Once the voter has the keys, they can vote.
The voting operation receives the vote. This operation encrypts the vote using the key pairs and the RSA algorithm and prints the encrypted vote.

The voter can reveal they keys.
The reveal operation prints the voter id, the d and p*q

The voter can also verify that the vote is correct.
The verification vote operation receives a list of votes, v, and encrypted votes, k, and prints whether or not the voter's vote is on this list.

When the voter finishes the vote, the system prints they key pairs.
```
Operation   | Identifier | Paramethers      | Output       | Error
------------------------------------------------------------------------------------------------------
Idenfity    |     I      | i t              | C            | E
Impersonate |     P      | i p q e d        | C            | E
Vote        |     V      | v                | C E(f(i,v))  | E
Reveal      |     R      |                  | C i d p*q    | E
Verify      |     C      | n v1 k1 v2 k2... | C            | E i E(f(i,v)) d p*q
Finalize    |     T      |                  | C p q e d    |
```

Example:

```
./election E
//Idenfity
I
1 10000
C
//Vote
V 4
C 11694666
//Reveal
R
C 1 87794669 227567929
//Verify
C 3 1 4543534 5 4324234 4 534534
E 1 11694666 87794669 227567929
//Verify
C 3 1 4234233 5 31323123 4 11694666
C
//Finalize
T
C 19183 11863 168001133 87794669
```


The Zone mode has the following operations:
First, the zone must be initialized with the initialization operation. This operation receives the number of voters, i, number of candidates, c, and the minimum value of prime numbers, t.

Once initialized, it can receive votes with the Receive Vote operation, followed by the voter's id and the encrypted vote.

You can list the encrypted votes already received with the blind list vote operation.

The open voting operation receives the id, the private key d and p*q. This operation decrypts the vote and checks the origin and its validity.

The publication of votes is the operation to list all votes.

The receipt protest verifies that the vote received by the user is what is stored.

The Finalize operation ends the zone that prints the election result.

The attack operation simulates an attack in the zone. The operation receives an id, a vote and a ciphered vote and replaces the vote of the real voter.

```
Operation        | Identifier | Paramethers        | Output            | Error
------------------------------------------------------------------------------------------------------
Initialize       |     I      | i c t              | C                 | E
Receive vote     |     R      | i E(f(i,v))        | C                 | E
Blind list votes |     L      |                    | C n k1 k2...      |
Open vote        |     A      | i d p*q            | C                 | E
Publish votes    |     P      |                    | C n v1 k1 v2 k2...| 
Receive protest  |     Q      | i E(f(i,v)) d p*q  | C                 | E
Finalize         |     T      |                    | C c c1 c2 c3...   | E
Attack           |     !      | i v_fake k_fake    | C                 | E
```


Example:

```
//Initialize
I
5 10  10000
C
//Receive vote
R 
1 11694666
C
//Blind list votes
L
C 1 11694666 
//Open vote
A 
1 87794669 227567929
C
//Publish votes
P
C 1 4 11694666
//Receive protest 
Q 
1 11694666 87794669 227567929
E
//Attack
!
1 3 35345345          
C
//Receive protest 
Q
1 11694666 87794669 227567929
C
//Finalize
T
C 10 0 0 0 0 1 0 0 0 0 0
```