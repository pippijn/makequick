#define resume(member)          n.member ()->accept (*this)
#define resume_if(member)       if (n.member ()) resume (member ())
#define resume_list()           for (std::vector<node_ptr>::iterator it = n.list.begin (), et = n.list.end (); it != et; ++it) if (*it) (*it)->accept (*this)
