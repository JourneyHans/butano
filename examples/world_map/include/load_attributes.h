/*
 * Copyright (c) 2020-2021 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#ifndef LOAD_ATTRIBUTES_H
#define LOAD_ATTRIBUTES_H

#include "bn_affine_bg_mat_attributes.h"

BN_CODE_IWRAM void load_attributes(bn::affine_bg_mat_attributes base_attributes,
                                   bn::affine_bg_mat_attributes* attributes);

#endif
