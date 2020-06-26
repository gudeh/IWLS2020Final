/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   node.cpp
 * Author: augusto
 * 
 * Created on 23 de Março de 2018, 17:00
 */

#include "AIG.h"

nodeAig::nodeAig() {
    this->id=0;
//    this->signal=-1;
}

nodeAig::~nodeAig(){
}

nodeAig::nodeAig(unsigned int param){
    this->id=param;
//    this->signal=-1;
}

void nodeAig::setId(unsigned int param){
    this->id=param;
}

unsigned int nodeAig::getId(){
    return this->id;
}

void nodeAig::setDepth(short int param){
    this->signal=param;
}

int nodeAig::getDepth(){
    return this->signal;
}

nodeAig* nodeAig::fixLSB(){
    return ((nodeAig*)(((uintptr_t)this) & ~01));
}

nodeAig* nodeAig::forceInvert(){
    return ((nodeAig*)(((uintptr_t)this) ^ 01));
}

void nodeAig::setSignal(int param){
    this->signal=param;
}

void nodeAig::setBitVector(unsigned long long int param) {
    this->bit_vector=param;
}

unsigned long long int nodeAig::getBitVector() {
    return this->bit_vector;
}

int nodeAig::getSignal(){
    return this->signal;
}