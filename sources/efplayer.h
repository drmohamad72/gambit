//
// FILE: efplayer.h -- Declaration of EFPlayer data type
//
// $Id$
//

#ifndef EFPLAYER_H
#define EFPLAYER_H

#ifdef __GNUG__
#pragma interface
#endif   // __GNUG__

class gRational;

class EFPlayer   {
  friend class BaseEfg;
  friend class Efg<double>;
  friend class Efg<gRational>;
  friend class BehavProfile<double>;
  friend class BehavProfile<gRational>;
  private:
    int number;
    gString name;
    BaseEfg *E;
    
    gBlock<Infoset *> infosets;

    EFPlayer(BaseEfg *e, int n) : number(n), E(e)  { }
    ~EFPlayer();

  public:
    BaseEfg *BelongsTo(void) const   { return E; }
  
    const gString &GetName(void) const   { return name; }
    void SetName(const gString &s)       { name = s; }

    bool IsChance(void) const      { return (number == 0); }

    int NumInfosets(void) const    { return infosets.Length(); }
    const gArray<Infoset *> &Infosets(void) const  { return infosets; }

    int GetNumber(void) const    { return number; }
};


#endif    // EFPLAYER_H


