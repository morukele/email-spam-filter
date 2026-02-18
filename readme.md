# Spam Filter

A recreational exploration of Naive Bayes Classifier for email spam filter using C++.

Inspired by Tsoding Daily's [video](https://www.youtube.com/watch?v=JsfOXk7qmSM) video on the subject.

> I am using boost library to access standard algorithms.

## Naive Bayes Probability Theory
The general probability formula is give as:

$$ P(A | B) = \dfrac{P(B | A) * P(A)}{P(B)} $$

Where:

- $P(A | B)$ is the probability of $A$ happening given $B$ has happened
- $P(B | A)$ is the probability of $B$ happening given $A$ has happened
- $P(A)$ is the probability of $A$ happening alone
- $P(B)$ is the probability of $B$ happening alone

For a spam filter, we have a set $ C = ${Spam , Ham}$ $, where $Ham$ is not spam.

With this, the general formula can be transformed as follows:

$$ P(C | D) = \dfrac{P(D | C) * P(C)}{P(D)} $$

Where:
- $D$ stands for document
- $P(C | D)$ is the probability of $C$ happening given $D$ has happened
- $P(D | C)$ is the probability of $D$ happening given $C$ has happened
- $P(C)$ is the probability of $C$ happening alone
- $P(D)$ is the probability of $D$ happening alone