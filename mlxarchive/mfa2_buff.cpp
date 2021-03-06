/*                  - Mellanox Confidential and Proprietary -
 *
 *  Copyright (C) 2019, Mellanox Technologies Ltd.  ALL RIGHTS RESERVED.
 *
 *  Except as specifically permitted herein, no portion of the information,
 *  including but not limited to object code and source code, may be reproduced,
 *  modified, distributed, republished or otherwise exploited in any form or by
 *  any means for any purpose without the prior written permission of Mellanox
 *  Technologies Ltd. Use of software subject to the terms and conditions
 *  detailed in the file "LICENSE.txt".
 *
 * pldm_buff.cpp
 *
 *  Created on: Feb 27, 2019
 *      Author: Samer Deeb
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>

#include "mfa2_buff.h"


Mfa2Buffer::Mfa2Buffer(): m_buff(NULL), m_pos(0), m_size(0) {
}

Mfa2Buffer::~Mfa2Buffer() {
    if (m_buff) {
        delete [] m_buff;
        m_buff = NULL;
    }
}

bool Mfa2Buffer::loadFile(const std::string& fname)
{
    // open the file:
    FILE * fp;

    fp = fopen(fname.c_str(), "rb");
    if (!fp) {
        return false;
    }

    //Get file length
    fseek(fp, 0, SEEK_END);
    m_size=ftell(fp);
    fseek(fp, 0, SEEK_SET);

    //Allocate memory
    if (m_buff) {
        delete [] m_buff;
        m_buff = NULL;
    }

    m_buff= new u_int8_t[m_size+1];
    if (!m_buff)
    {
        return false;
    }

    //Read file contents into buffer
    if (fread(m_buff, m_size, 1, fp)) {} // TODO: temporary fix - handle the return value!
    fclose(fp);

    return true;
}


void Mfa2Buffer::read(u_int8_t & val) {
    val = *(m_buff + m_pos);
    m_pos += sizeof(u_int8_t);
}

void Mfa2Buffer::read(u_int16_t & val) {
    val = *(u_int16_t *)(m_buff + m_pos);
    m_pos += sizeof(u_int16_t);
    val = __le16_to_cpu(val);
}

void Mfa2Buffer::read(u_int32_t & val) {
    val = *(u_int32_t *)(m_buff + m_pos);
    m_pos += sizeof(u_int32_t);
    val = __le32_to_cpu(val);
}

void Mfa2Buffer::read(std::string& str, size_t str_size) {
    u_int8_t *arr = new u_int8_t[str_size];
    read(arr, str_size);
    str = std::string(reinterpret_cast<char const *>(arr), str_size);
    delete [] arr;
}

void Mfa2Buffer::read(u_int8_t *arr, size_t arr_size) {
    memcpy(arr, m_buff + m_pos, arr_size);
    m_pos += arr_size;
}

int Mfa2Buffer::seek(long offset, int whence) {
    long new_pos = -1;

    switch(whence) {
    case SEEK_SET:
        new_pos = offset;
        break;
    case SEEK_CUR:
        new_pos = m_pos + offset;
        break;
    case SEEK_END:
        new_pos = m_size + offset;
        break;
    }

    if(new_pos < 0 || new_pos > m_size) {
        return -1;
    }
    m_pos = new_pos;
    return 0;
}

long Mfa2Buffer::tell() {
    return m_pos;
}

void Mfa2Buffer::rewind() {
    m_pos = 0;
}
