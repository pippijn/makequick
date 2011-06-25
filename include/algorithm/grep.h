#pragma once

template<typename InputIterator, typename OutputIterator, typename UnaryOperator>
OutputIterator
grep (InputIterator first1, InputIterator last1, OutputIterator result, UnaryOperator op)
{
  while (first1 != last1)
    {
      if (op (*first1))
        *result++ = *first1;
      ++first1;
    }
  return result;
}
