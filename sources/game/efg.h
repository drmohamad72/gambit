//
// FILE: efg.h -- Declaration of extensive form data type
//
// $Id$
//

#ifndef EFG_H
#define EFG_H

#include "base/base.h"
#include "math/gnumber.h"
#include "math/gpvector.h"

class EFPlayer;
class Infoset;
class Node;
class Action;
class EFSupport;
class Lexicon;
class Nfg;
template <class T> class BehavProfile;
template <class T> class MixedProfile;
template <class T> class PureBehavProfile;

namespace Efg {
  class Outcome {
    friend class Game;

  private:
    int m_outcomeID;
    Game *m_efg;

    int GetID(void) const { return m_outcomeID; }

  protected:
    Outcome(int, Game *);
    
  public:
    // Default constructor for use with gArray
    Outcome(void) : m_outcomeID(0), m_efg(0) { }
  
    bool operator==(const Outcome &p_outcome) const
    { return (m_outcomeID == p_outcome.m_outcomeID &&
	      m_efg == p_outcome.m_efg); }
    bool operator!=(const Outcome &p_outcome) const
    { return !(*this == p_outcome); }

    Game *GetGame(void) const { return m_efg; }
    bool IsNull(void) const { return (m_outcomeID == 0); }
  };

  inline gOutput &operator<<(gOutput &p_file, const Outcome &)
    { return p_file; }

  class Game {
  protected:
    int GetOutcomeID(const Outcome &p_outcome) const
      { return p_outcome.GetID(); }

  public:
    class Exception : public gException   {
    public:
      virtual ~Exception()   { }
      gText Description(void) const    { return "Efg error"; }
    };

    virtual ~Game() { }
    
    virtual const gText &GetTitle(void) const = 0;
    virtual long RevisionNumber(void) const = 0;
    
    virtual int NumPlayers(void) const = 0;
    virtual const gArray<EFPlayer *> &Players(void) const = 0; 
    virtual EFPlayer *GetChance(void) const = 0;

    virtual int ProfileLength(void) const = 0;
    virtual int TotalNumInfosets(void) const = 0;

    virtual gArray<int> NumInfosets(void) const = 0;
    virtual int NumPlayerInfosets(void) const = 0;
    virtual int NumChanceInfosets(void) const = 0;
    virtual gPVector<int> NumActions(void) const = 0;
    virtual int NumPlayerActions(void) const = 0;
    virtual int NumChanceActions(void) const = 0;
    virtual gPVector<int> NumMembers(void) const = 0;
    virtual gBlock<Infoset *> Infosets(void) const = 0;
  
    virtual gNumber GetChanceProb(Infoset *, int) const = 0;
    virtual gNumber GetChanceProb(const Action *) const = 0;

    virtual Node *RootNode(void) const = 0;
    virtual bool IsSuccessor(const Node *n, const Node *from) const = 0;
    virtual bool IsPredecessor(const Node *n, const Node *of) const = 0;
    virtual gArray<int> PathToNode(const Node *) const = 0;
    virtual const gArray<Node *> &Children(const Node *n) const = 0;
    virtual int  NumChildren(const Node *n) const = 0;
    virtual gList<Node *> TerminalNodes(void) const = 0;  

    virtual bool IsLegalSubgame(Node *n) = 0;
    virtual void MarkSubgames(const gList<Node *> &list) = 0;
    virtual bool MarkSubgame(Node *n) = 0;
    virtual void UnmarkSubgame(Node *n) = 0;
    virtual void UnmarkSubgames(Node *n) = 0;
    
    Outcome GetNullOutcome(void) const
      { return Outcome(0, const_cast<Game *>(this)); }
    virtual Outcome GetOutcome(const Node *const) const = 0;
    virtual void SetOutcome(Node *, const Outcome &) = 0;

    virtual void SetOutcomeName(const Outcome &, const gText &) = 0;
    virtual const gText &GetOutcomeName(const Outcome &) const = 0;

    virtual void DeleteOutcome(const Outcome &) = 0;

    virtual gNumber MinPayoff(int pl = 0) const = 0;
    virtual gNumber MaxPayoff(int pl = 0) const = 0;
    virtual bool IsConstSum(void) const = 0;

    virtual void SetPayoff(const Outcome &, int pl, const gNumber &value) = 0;
    virtual gNumber Payoff(const Outcome &, const EFPlayer *) const = 0;
    virtual gNumber Payoff(const Node *, const EFPlayer *) const = 0;
    virtual gArray<gNumber> Payoff(const Outcome &) const = 0;
    virtual void Payoff(const gPVector<int> &profile,
			gVector<gNumber> &payoff) const = 0;
    virtual void Payoff(const gArray<gArray<int> *> &profile,
			gArray<gNumber> &payoff) const = 0;

    virtual Nfg *AssociatedNfg(void) const = 0;
    virtual Nfg *AssociatedAfg(void) const = 0;
    virtual Lexicon *GetLexicon(void) const = 0;
  };
}  // namespace Efg

class EfgClient;

namespace FullEfgNamespace {
  class Outcome;

  class FullEfg : public Efg::Game  {
  private:
    // this is used to track memory leakage; #define MEMCHECK to use it
#ifdef MEMCHECK
    static int _NumObj;
#endif   // MEMCHECK

    friend class EfgFileReader;
    friend class EfgFile;
    friend class Nfg;
    friend class Node;
    friend class BehavProfile<double>;
    friend class BehavProfile<gRational>;
    friend class BehavProfile<gNumber>;

  protected:
    bool sortisets;
    mutable bool m_dirty;
    mutable long m_revision;
    mutable long m_outcome_revision;
    gText title, comment;
    gBlock<EFPlayer *> players;
    gBlock<Outcome *> outcomes;
    Node *root;
    EFPlayer *chance;
    mutable Nfg *afg;
    mutable Lexicon *lexicon;
    gList<EfgClient *> m_clients;

    // this is for use with the copy constructor
    void CopySubtree(Node *, Node *);

    void CopySubtree(Node *, Node *, Node *);
    void MarkSubtree(Node *);
    void UnmarkSubtree(Node *);

    void SortInfosets(void);
    void NumberNodes(Node *, int &);
  
    void DeleteLexicon(void) const;

    Efg::Outcome NewOutcome(int index);

    void WriteEfgFile(gOutput &, Node *) const;

    void Payoff(Node *n, gNumber, const gPVector<int> &, gVector<gNumber> &) const;
    void Payoff(Node *n, gNumber, const gArray<gArray<int> *> &, gArray<gNumber> &) const;
  
    void InfosetProbs(Node *n, gNumber, const gPVector<int> &, gPVector<gNumber> &) const;
    
    
    // These are used in identification of subgames
    void MarkTree(Node *, Node *);
    bool CheckTree(Node *, Node *);
    void MarkSubgame(Node *, Node *);

    // Recursive calls
    void DescendantNodes(const Node *, const EFSupport &, 
			 gList<Node *> &) const;
    void NonterminalDescendants(const Node *, const EFSupport&, 
				gList<const Node*> &) const;
    void TerminalDescendants(const Node *, const EFSupport&, 
			     gList<Node *> &) const;

    void NotifyClients(bool, bool) const;
    
  public:
    FullEfg(void);
    FullEfg(const FullEfg &, Node * = 0);
    virtual ~FullEfg();

    // TITLE ACCESS AND MANIPULATION
    void SetTitle(const gText &s);
    const gText &GetTitle(void) const;

    void SetComment(const gText &);
    const gText &GetComment(void) const;

    // WRITING DATA FILES
    void WriteEfgFile(gOutput &p_file, int p_nDecimals) const;

    // DATA ACCESS -- GENERAL INFORMATION
    bool IsConstSum(void) const; 
    long RevisionNumber(void) const { return m_revision; }
    bool IsDirty(void) const { return m_dirty; }
    void SetIsDirty(bool p_dirty) { m_dirty = p_dirty; }
    gNumber MinPayoff(int pl = 0) const;
    gNumber MaxPayoff(int pl = 0) const;
  
    Node *RootNode(void) const;
    bool IsSuccessor(const Node *n, const Node *from) const;
    bool IsPredecessor(const Node *n, const Node *of) const;
    gArray<int> PathToNode(const Node *) const;
    //    const Node* Consequence(const Node&, Action&) const;
    gList<Node *> DescendantNodes(const Node&, const EFSupport&) const;
    gList<const Node*> NonterminalDescendants(const Node&, 
					      const EFSupport&) const;
    gList<Node *> TerminalDescendants(const Node&, 
				      const EFSupport&) const;
    gList<Node *> TerminalNodes(void) const;
    gList<Infoset*> DescendantInfosets(const Node&, const EFSupport&) const;
    const gArray<Node *> &Children(const Node *n) const;
    int NumChildren(const Node *n) const;

    // DATA ACCESS -- PLAYERS
    int NumPlayers(void) const;
    EFPlayer *GetChance(void) const;
    EFPlayer *NewPlayer(void);
    const gArray<EFPlayer *> &Players(void) const  { return players; }

    // DATA ACCESS -- INFOSETS
    gBlock<Infoset *> Infosets(void) const;

    // DATA ACCESS -- OUTCOMES
    int NumOutcomes(void) const;
    Efg::Outcome GetOutcome(int index) const;
    Efg::Outcome NewOutcome(void);
    void DeleteOutcome(const Efg::Outcome &);

    Efg::Outcome GetOutcome(const Node *const) const;
    void SetOutcome(Node *, const Efg::Outcome &);

    void SetOutcomeName(const Efg::Outcome &, const gText &);
    const gText &GetOutcomeName(const Efg::Outcome &) const;
 
    // EDITING OPERATIONS
    Infoset *AppendNode(Node *n, EFPlayer *p, int br);
    Infoset *AppendNode(Node *n, Infoset *s);
    Node *DeleteNode(Node *n, Node *keep);
    Infoset *InsertNode(Node *n, EFPlayer *p, int br);
    Infoset *InsertNode(Node *n, Infoset *s);

    Infoset *CreateInfoset(EFPlayer *pl, int br);
    bool DeleteEmptyInfoset(Infoset *);
    void DeleteEmptyInfosets(void);
    Infoset *JoinInfoset(Infoset *s, Node *n);
    Infoset *LeaveInfoset(Node *n);
    Infoset *SplitInfoset(Node *n);
    Infoset *MergeInfoset(Infoset *to, Infoset *from);

    Infoset *SwitchPlayer(Infoset *s, EFPlayer *p);
  
    Node *CopyTree(Node *src, Node *dest);
    Node *MoveTree(Node *src, Node *dest);
    Node *DeleteTree(Node *n);

    Action *InsertAction(Infoset *s);
    Action *InsertAction(Infoset *s, const Action *at);
    Infoset *DeleteAction(Infoset *s, const Action *a);

    void Reveal(Infoset *, const gArray<EFPlayer *> &);

    void SetChanceProb(Infoset *, int, const gNumber &);
    gNumber GetChanceProb(Infoset *, int) const;
    gNumber GetChanceProb(const Action *) const;
    gArray<gNumber> GetChanceProbs(Infoset *) const;

    void SetPayoff(const Efg::Outcome &, int pl, const gNumber &value);
    gNumber Payoff(const Efg::Outcome &, const EFPlayer *) const;
    gNumber Payoff(const Node *, const EFPlayer *) const;
    gArray<gNumber> Payoff(const Efg::Outcome &) const;

    void InitPayoffs(void) const;
  
    bool IsLegalSubgame(Node *n);
    void MarkSubgames(const gList<Node *> &list);
    bool MarkSubgame(Node *n);
    void UnmarkSubgame(Node *n);
    void UnmarkSubgames(Node *n);

    int ProfileLength(void) const;
    int TotalNumInfosets(void) const;

    gArray<int>   NumInfosets(void) const;  // Does not include chance infosets
    int           NumPlayerInfosets(void) const;
    int           NumChanceInfosets(void) const;
    gPVector<int> NumActions(void) const;
    int           NumPlayerActions(void) const;
    int           NumChanceActions(void) const;
    gPVector<int> NumMembers(void) const;
  
    // COMPUTING VALUES OF PROFILES
    void Payoff(const gPVector<int> &profile, gVector<gNumber> &payoff) const;
    void Payoff(const gArray<gArray<int> *> &profile,
		gArray<gNumber> &payoff) const;

    void InfosetProbs(const gPVector<int> &profile, gPVector<gNumber> &prob) const;
    
    Nfg *AssociatedNfg(void) const;
    Nfg *AssociatedAfg(void) const;
    Lexicon *GetLexicon(void) const { return lexicon; }

    friend Nfg *MakeReducedNfg(const EFSupport &);
    friend Nfg *MakeAfg(const Efg::Game &);

    // These are auxiliary functions used by the .efg file reader code
    Infoset *GetInfosetByIndex(EFPlayer *p, int index) const;
    Infoset *CreateInfosetByIndex(EFPlayer *p, int index, int br);
    Efg::Outcome GetOutcomeByIndex(int index) const;
    Efg::Outcome CreateOutcomeByIndex(int index);
    void Reindex(void);
    Infoset *CreateInfoset(int n, EFPlayer *pl, int br);

    void RegisterClient(EfgClient *);
    void UnregisterClient(EfgClient *);
  };
}

//
// temporary provision: import FullEfg class name into global namespace
//
using FullEfgNamespace::FullEfg;
using FullEfgNamespace::MakeReducedNfg;
using FullEfgNamespace::MakeAfg;

//#include "behav.h"

#include "efplayer.h"
#include "infoset.h"
#include "node.h"

FullEfg *ReadEfgFile(gInput &);

template <class T> class PureBehavProfile   {
  protected:
    const Efg::Game *E;
    gArray<gArray<const Action *> *> profile;

    //    void IndPayoff(const Node *n, const int &pl, const T, T &) const;
    // This aims at efficiency, but leads to a problem described in behav.imp

    void Payoff(const Node *n, const T, gArray<T> &) const;
    void InfosetProbs(Node *n, T, gPVector<T> &) const;

  public:
    PureBehavProfile(const Efg::Game &);
    PureBehavProfile(const PureBehavProfile<T> &);
    ~PureBehavProfile();

    // Operators
    PureBehavProfile<T> &operator=(const PureBehavProfile<T> &);
    T operator()(Action *) const;

    // Manipulation
    void Set(const Action *);
    void Set(const EFPlayer *, const gArray<const Action *> &);

    // Information
    const Action *GetAction(const Infoset *) const;
    
    const T Payoff(const Efg::Outcome &, const int &pl) const;
    const T ChanceProb(const Infoset *, const int &act) const;
    
    const T Payoff(const Node *, const int &pl) const;
  //    T    Payoff(const int &pl) const;
    void Payoff(gArray<T> &payoff) const;
    void InfosetProbs(gPVector<T> &prob) const;
    Efg::Game &GetGame(void) const   { return const_cast<Efg::Game &>(*E); }
};


#include "efgutils.h"

//
// An abstract base class for notification of other data structures when
// changes occur in an extensive form
//
class EfgClient {
friend class FullEfg;
private:
  FullEfg *m_efg;
  
protected:
  EfgClient(FullEfg *);
  virtual ~EfgClient();

  virtual void OnTreeChanged(bool p_nodesChanged, bool p_infosetsChanged) = 0;
  void OnEfgDestructed(void);
};

#endif   // EFG_H

