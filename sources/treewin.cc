//
// FILE: Treewin.cc -- Type-specific portion of the extensive form rendering code.
// 
// $Id$
//

#include "wx.h"
#include "wx_form.h"
#include "wxmisc.h"
#include "efg.h"
#include "infoset.h"
#include "node.h"
#include "gmisc.h"
#include "treewin.h"
#include "extshow.h"


//=====================================================================
//                      TREEWINDOW MEMBER FUNCTIONS
//=====================================================================

//---------------------------------------------------------------------
//                TREEWINDOW: CONSTRUCTOR AND DESTRUCTOR
//---------------------------------------------------------------------
//************************* private constructor & assignment *******
template <class T>
TreeWindow<T>::TreeWindow(const TreeWindow<T> &t):BaseTreeWindow(t),ef(t.ef)
{assert(0);}
template <class T>
void TreeWindow<T>::operator=(const TreeWindow<T> &)
{assert(0);}


//************************* normal constructor ********************
template <class T>
TreeWindow<T>::TreeWindow(Efg<T> &ef_,EFSupport * &disp,ExtensiveShow<T> *frame_)
												:	ef(ef_), frame(frame_), BaseTreeWindow(ef_,disp,frame_)
{ }

template <class T>
TreeWindow<T>::~TreeWindow()
{
#ifdef SUBGAMES
if (subgame!=1) ;		// if I am a subgame, notify parent of my death
//	 ((ExtensiveShow<T> *)frame->parent)->tw->CloseSubgame(subgame);
else
#endif
}

template <class T>
double TreeWindow<T>::ProbAsDouble(const Node *n,int action) const
{
return (double)frame->BranchProb(n,action);
}

template <class T>
gString TreeWindow<T>::OutcomeAsString(const Node *n) const
{
if (n->GetOutcome())
{
	EFOutcome *tv = n->GetOutcome();
	const gVector<T> &v=ef.Payoff(tv);
//	ToStringPrecision(2);
	gString tmp="(";
	for (int i=v.First();i<=v.Last();i++)
	{
		if (i!=1) tmp+=",";
		if (draw_settings.ColorCodedOutcomes())
			tmp+=("\\C{"+ToString(draw_settings.GetPlayerColor(i))+"}");
		tmp+=ToString(v[i]);
	}
	if (draw_settings.ColorCodedOutcomes()) tmp+=("\\C{"+ToString(WX_COLOR_LIST_LENGTH-1)+"}");
	tmp+=")";

	return tmp;
}
else
	return "";
}

//***********************************************************************
//                      TREE-OUTCOME MENU HANDLER
//***********************************************************************
#include "outcomed.h"

template<class T>
void TreeWindow<T>::tree_outcomes(const gString out_name)
{
if (!outcome_dialog) // no window for this efg
{
	outcome_dialog=new OutcomeDialog<T>(ef,this);
	return;
}
else	// going to a new one by clicking on an outcome
{
	if (out_name!="")	outcome_dialog->SetOutcome(out_name);
}
}

//***********************************************************************
//                      NODE-PROBS MENU HANDLER
//***********************************************************************
// The text input fields are stacked vertically up to ENTRIES_PER_DIALOG.
// If there are more than ENTRIES_PER_DIALOG actions for this infoset,
// consequtive dialogs will be created.
#define ENTRIES_PER_DIALOG	10

template<class T> void TreeWindow<T>::action_probs(void)
{
Node *n=cursor;
int 	i;

if (!n->GetInfoset())
{
	wxMessageBox("Probs can only be set for a CHANCE player","Error",wxOK | wxCENTRE,frame);
	return;
}

if (!n->GetPlayer()->IsChance())	// if this is not a chance player
{
	wxMessageBox("Probs can only be set for a CHANCE player","Error",wxOK | wxCENTRE,frame);
	return;
}

int num_actions=cursor->NumChildren();
int num_d=num_actions/ENTRIES_PER_DIALOG-((num_actions%ENTRIES_PER_DIALOG) ? 0 : 1);

for (int d=0;d<=num_d;d++)
{
	MyDialogBox *node_probs_dialog=new MyDialogBox(frame,"Node Probabilities");
	int actions_now=gmin(num_actions-d*ENTRIES_PER_DIALOG,ENTRIES_PER_DIALOG);
	char **prob_vector=new char *[actions_now+1];
	for (i=1;i<=actions_now;i++)
	{
		T temp_p=((ChanceInfoset<T> *)(n->GetInfoset()))->GetActionProb(i+d*ENTRIES_PER_DIALOG);
		gString temp_s=ToString(temp_p);
		prob_vector[i]=new char[temp_s.length()+1];
		strcpy(prob_vector[i],temp_s);
		node_probs_dialog->Add(wxMakeFormString("",&(prob_vector[i]),wxFORM_TEXT,NULL,NULL,wxVERTICAL,80));
		node_probs_dialog->Add(wxMakeFormNewLine());
	}
	if (num_actions-(d+1)*ENTRIES_PER_DIALOG>0)
		node_probs_dialog->Add(wxMakeFormMessage("Continued..."));

	node_probs_dialog->Go();
	if (node_probs_dialog->Completed()==wxOK)
	{
		T dummy;
		for (i=1;i<=actions_now;i++)
			((ChanceInfoset<T> *)n->GetInfoset())->SetActionProb(i+d*ENTRIES_PER_DIALOG,FromString(prob_vector[i],dummy));
		outcomes_changed=TRUE;  // game changed -- delete solutions, etc
	}
	for (i=1;i<=actions_now;i++) delete [] prob_vector[i];delete [] prob_vector;
	delete node_probs_dialog;
}
}


//***********************************************************************
//                      FILE-SAVE MENU HANDLER
//***********************************************************************
template <class T> Efg<T> *CompressEfg(const Efg<T> &, const EFSupport &);
template <class T> void TreeWindow<T>::file_save(void)
{
gString filename=frame->GetFileName();
gString s=wxFileSelector("Save data file",wxPathOnly(filename),wxFileNameFromPath(filename),".efg", "*.efg",wxSAVE|wxOVERWRITE_PROMPT);
if (s!="")
{
	// Change description if saving under a different filename
	if (filename!="untitled.efg" && s!=filename) tree_label();
	gFileOutput out((const char *)s);
	// Compress the efg to the current support
	Efg<T> *E=CompressEfg(ef,*frame->GetSupport(0));
	E->WriteEfgFile(out);
	delete E;
	frame->SetFileName(s);
}
}

#ifdef __GNUG__
	#define TEMPLATE template
#elif defined __BORLANDC__
	class gList<Node *>;
	class gList<BehavProfile<double> >;
	class gNode<BehavProfile<double> >;
	class gList<BehavProfile<gRational> >;
	class gNode<BehavProfile<gRational> >;
	#pragma option -Jgd
	#define TEMPLATE
#endif   // __GNUG__, __BORLANDC__
//TEMPLATE class gList<OutcomeWindowS>;
TEMPLATE class TreeWindow<double> ;
TEMPLATE class TreeWindow<gRational> ;



