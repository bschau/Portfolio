/*
 *  Constants.h
 *  iStar
 *
 *  Created by Brian Schau on 18/05/09.
 *  Copyright 2009 Folera.Com. All rights reserved.
 *
 */
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define PLANETS 0
#define PLANETM (PLANETS + 1)
#define PLANETL (PLANETM + 1)

#define DEFLECTORS 0
#define DEFLECTORM (DEFLECTORS + 1)
#define DEFLECTORL (DEFLECTORM + 1)

#define WORMHOLEMASS 4000.0
#define WORMHOLEGRAVITY 0.05
#define WORMHOLESIZE 16.0

#define WALLHS 0
#define WALLHM (WALLHS + 1)
#define WALLHL (WALLHM + 1)
#define WALLVS (WALLHL + 1)
#define WALLVM (WALLVS + 1)
#define WALLVL (WALLVM + 1)

#define PADDLEVSS 0
#define PADDLEVSF (PADDLEVSS + 1)
#define PADDLEVMS (PADDLEVSF + 1)
#define PADDLEVMF (PADDLEVMS + 1)
#define PADDLEVLS (PADDLEVMF + 1)
#define PADDLEVLF (PADDLEVLS + 1)
#define PADDLEHSS (PADDLEVLF + 1)
#define PADDLEHSF (PADDLEHSS + 1)
#define PADDLEHMS (PADDLEHSF + 1)
#define PADDLEHMF (PADDLEHMS + 1)
#define PADDLEHLS (PADDLEHMF + 1)
#define PADDLEHLF (PADDLEHLS + 1)
#define PADDLESIZE 40.0

/* Indices into spritesheet where to find graphics */
#define SPRPLAYERIDX 0
#define SPRPLANETIDX 1
#define SPRDEFLECTORIDX 4
#define SPRWORMHOLEIDX 7
#define SPRWALLIDX 12
#define SPRPADDLEIDX 18
#define SPRFIREPADIDX 24

#endif

