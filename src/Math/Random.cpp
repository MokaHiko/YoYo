#include "Random.h"

#include <objbase.h>
#include <Core/Log.h>

namespace yoyo
{
	PrimeSearch::PrimeSearch(int n_elements)
		: m_n_elements(n_elements)
	{
		assert(n_elements > 0 && "Cant' do prime search if no elements, buddy boy!");

		m_a = m_random_engine.Next();
		m_b = m_random_engine.Next();
		m_c = m_random_engine.Next();

		m_skip = (m_a * (m_n_elements * m_n_elements)) + (m_b * m_n_elements) + m_c;
		m_skip &= ~0xc0000000;

		m_prime = FindNextPrime(n_elements);

		// Prevent collapse to 0
		int test = (m_skip % m_prime);
		if (!test)
		{
			m_skip++;
		}
	}

	int PrimeSearch::GetNext(bool restart)
	{
		if (restart)
		{
			Restart();
		}

		if (m_searches == m_prime)
		{
			return -1;
		}

		m_nextMember += m_skip;
		m_nextMember %= m_prime;
		m_searches++;

		return m_nextMember < m_n_elements ? m_nextMember : GetNext();
	}

	void PrimeSearch::Restart()
	{
		m_nextMember = 0;
		m_searches = 0;
	}

	int PrimeSearch::FindNextPrime(int n)
	{
		// Base case
		if (n <= 1)
		{
			return 2;
		}

		bool found = false;
		while (!found)
		{
			n++;
			if (IsPrime(n))
			{
				return n;
			}
		}

		return 0;
	}

	int PrimeSearch::IsPrime(int n)
	{
		// Corner cases
		if (n <= 1)
		{
			return false;
		}
		if (n <= 3)
		{
			return false;
		}

		if (n % 2 == 0 || n % 3 == 0)
		{
			return false;
		}

		for (int i = 5; i * i <= n; i = i + 6)
		{
			if (n % i == 0 || n % (i + 2) == 0)
			{
				return false;
			}
		}

		return true;
	}
}