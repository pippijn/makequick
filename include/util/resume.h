#define RESUME(member)          n.member ()->accept (*this)
#define RESUME_IF(member)       if (n.member ()) RESUME (member ())
#define resume_list()           for (std::vector<node_ptr>::iterator it = n.list.begin (), et = n.list.end (); it != et; ++it) if (*it) (*it)->accept (*this)
