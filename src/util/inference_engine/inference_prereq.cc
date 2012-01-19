#include <boost/regex.hpp>

#include "util/inference_engine.h"


template<typename T>
inference_engine::prerequisite::file_t<T>::file_t (T const &data)
  : data (data)
{
}


#include "string.h"
#include "regex.h"
#include "wildcard.h"

#include "../inline_path.h"
