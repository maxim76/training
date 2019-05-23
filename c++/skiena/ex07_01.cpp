/*
Chapter 7 Exercises
Backtracking
7-1. [3] A derangement is a permutation p of {1, ..., n} such that no item is in its proper
position, i.e. pi = i for all 1 ≤ i ≤ n. Write an efficient backtracking program with
pruning that constructs all the derangements of n items.
*/

#include <stdio.h>

#define NMAX 3
#define MAXCANDIDATES	NMAX

bool finished = false;

void construct_candidates(int a[], int k, int n, int c[], int *ncandidates)
{
	int i; /* counter */
	bool in_perm[NMAX]; /* who is in the permutation? */
	for (i = 1; i < NMAX; i++) in_perm[i] = false;
	for (i = 0; i < k; i++) in_perm[a[i]] = true;
	*ncandidates = 0;
	for (i = 1; i <= n; i++)
		if (in_perm[i] == false) {
			c[*ncandidates] = i;
			*ncandidates = *ncandidates + 1;
		}
}

void process_solution(int a[], int k)
{
	int i; /* counter */
	for (i = 1; i <= k; i++) printf(" %d", a[i]);
	printf("\n");
}

bool is_a_solution(int a[], int k, int n)
{
	return (k == n);
}

void backtrack(int a[], int k, int input)
{
	int c[MAXCANDIDATES];
	int ncandidates;
	int i;

	if (is_a_solution(a, k, input))
		process_solution(a, k);
	else
	{
		k += 1;
		construct_candidates(a, k, input, c, &ncandidates);
		for (i = 0; i < ncandidates; i++)
		{
			a[k] = c[i];
			//make_move(a,k,input);
			backtrack(a,k,input);
			//unmake_move(a, k, input);
			if (finished) return;
		}
	}
}


int main()
{
	int a[NMAX] = { 1,2,3 };
	backtrack(a, 0, NMAX);
}
