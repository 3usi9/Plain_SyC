#include "asm_flow_optimize.h"

void Recompute_Lives(std::list<asm_basic_block_t*>& blks)
{
  // Pass 2.2, Redo LiveVariable Analysis
  // Pass 2.2.1 Find [UEVar] and [VarKill] for each block
  for(auto p = blks.begin();
      p != blks.end();
      ++p)
    {
      (*p)->get_register_dependency();
      (*p)->liveOut.clear();
    }
        
  // Pass 2.2.2 Find [LiveOut] for each block
  // LiveIn[Blk] = UEVar[Blk] UNION (LiveOut[Blk] INTERSECT NEG(VarKill[Blk]) )

  bool modified = true;
  while(modified)
    {
      modified = false;

      for(auto p = blks.begin();
	  p != blks.end();
	  ++p)
	{
	  modified |= (*p)->compute_liveout();
	}
    }
  // Pass 2.2.2+, Finalize [LiveIn] for each block
  for(auto it : blks)
    {
      it->Finalize_LiveIn();
    }
}
void ASM_Flow_Optimize(std::list<asm_basic_block_t*>& blks)
{
  // Pass 1, Eliminate Not-Referenced Assignments
  {
      
    bool changed = true;
    int cnt = 0;
    while(changed)
      {
	cnt++;
	changed = false;

	// Pass 2.2, Redo LiveVariable Analysis
	// Pass 2.2.1 Find [UEVar] and [VarKill] for each block
	for(auto p = blks.begin();
	    p != blks.end();
	    ++p)
	  {
	    (*p)->get_register_dependency();
	    (*p)->liveOut.clear();
	  }
        
	// Pass 2.2.2 Find [LiveOut] for each block
	// LiveIn[Blk] = UEVar[Blk] UNION (LiveOut[Blk] INTERSECT NEG(VarKill[Blk]) )

	bool modified = true;
	while(modified)
	  {
	    modified = false;

	    for(auto p = blks.begin();
		p != blks.end();
		++p)
	      {
		modified |= (*p)->compute_liveout();
	      }
	  }
	// Pass 2.2.2+, Finalize [LiveIn] for each block
	for(auto it : blks)
	  {
	    it->Finalize_LiveIn();
	  }
	
	for(auto p = blks.begin();
	    p != blks.end();
	    ++p)
	  {
	    std::set<std::string> liveNow = (*p)->liveOut;
	    auto q = (*p)->end;
	    if(q == (*p)->begin)
	      // Empty Block
	      {
		continue;
	      }

	    q--; // Safe
	    while(true)
	      {
		myAssert(typeid(*(instr_t*)(*q)) == typeid(instr_t), ASSERT_EXIT_CODE_88);
		auto& asmop = *(instr_t*)(*q);
		if(mySetIntersect(Get_Killed_vars(asmop), liveNow) == std::set<std::string>({}) &&
		   Get_Killed_vars(asmop) != std::set<std::string>({}))
		  {
		    if(asmop.condition_string != "")
		      {
			// do nothing
		      }
		    else
		      {
			// If it's CALL Operation, might have side effects
			if(asmop.mnemonic == MNEMO_BL)
			  {
			    // do nothing
			  }
			else
			  {
			    // std::cerr<<"KILL INSTRUCTION:";
			    // asmop.print(std::cerr);
			    // std::cerr<<"Where LIVENOW = ";
			    // for(auto it : liveNow)
			    //   {
			    // 	std::cerr<<it<<" ";
			    //   }
			    // std::cerr<<std::endl;
			    // std::cerr<<"AND CURRENT BLOCK = "<<std::endl;
			    // (*p)->print(std::cerr);
			    asmop = instr_t(MNEMO_NOP);
			    changed = true;
			  }
		      }
		  }
		// Remove Define from LiveNow
		std::set<std::string> se = Get_Killed_vars(asmop);
		for(auto it : se)
		  liveNow.erase(it);
	    
		// Insert Reference into LiveNow
		se = Get_Referenced_vars(asmop);
		// printf("References for ");
		// asmop.print(std::cerr);
		// for(auto it : se)
		//   {
		//     std::cerr<<it<<" ";
		//   }
		// std::cerr<<std::endl;
		for(auto it : se)
		  liveNow.insert(it);

		if(q == (*p)->begin)
		  break;
		else
		  q--;
	      }
	  }
      }
  }    

  // Pass 2, LiveRange AliasAnalyze
  {
    // Pass 2.1, Numbering each instruction
    int linum = 1;
    for(auto it : blks)
      {
	for(auto q = it->begin;
	    q != it->end;
	    q++)
	  {
	    (*q)->line_number = linum++;
	  }
      }
    // Pass 2.2, Create LiveRangeSet and Replace Alias
    for(auto it : blks)
      {
	LiveRangeSet_t LRSet(*it);
	for(auto p = it->begin;
	    p != it->end;
	    ++p)
	  {
	    if((*p)->mnemonic == MNEMO_MOV && LRSet.Check_Alias_Replacable(**p))
	      {
		LiveRange_t* inner = LRSet.get_Defined_LiveRange((*p)->line_number, (*p)->dest.name);
		inner->rewrite_reference((*p)->ops1.name);
	      }
	  }
      }
    for(auto it : blks)
      {
	// it->print(std::cerr);
    	// for(auto p = it->begin;
    	//     p != it->end;
    	//     ++p)
    	//   {
    	//     (*p)->print(std::cout);
    	//   }
      }

    // pass 2.3, Create LiveRangeSet and Rename free regs
    for(auto it : blks)
      {
    	LiveRangeSet_t LRSet(*it);
    	for(auto p = it->begin;
    	    p != it->end;
    	    ++p)
    	  {
    	    LRSet.Try_Rename_Register(**p);
    	  }
      }
  }
  Recompute_Lives(blks);
  { // Pass 3, (Local) Available Expression Substitution
    // Pass 3.1, Numbering each instruction
    int linum = 1;
    for(auto it : blks)
      {
  	for(auto q = it->begin;
  	    q != it->end;
  	    q++)
  	  {
  	    (*q)->line_number = linum++;
  	  }
      }

    for(auto it : blks)
      {
    	LiveRangeSet_t LRSet(*it);
  	_asm_ExpressionSet_t exprSet;
  	exprSet.Proceed(it, LRSet);
      }
	
  }
}
