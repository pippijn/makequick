#pragma once

#include "annotation_map.h"
#include "exception.h"
#include "local.h"
#include "node.h"
#include "node_type.h"
#include "phase.h"
#include "phases.h"
#include "sighandler.h"
#include "visitor.h"
#include "ylcode.h"

#define resume(member)          n.member = n.member->accept (*this)
#define resume_if(member)       if (n.member) resume (member)
#define resume_list()           for (std::vector<node_ptr>::iterator it = n.list.begin (), et = n.list.end (); it != et; ++it) if (*it) (*it)->accept (*this)

using namespace nodes;
using namespace tokens;
