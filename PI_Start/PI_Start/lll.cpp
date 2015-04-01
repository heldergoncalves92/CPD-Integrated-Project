//
//  lll.cpp
//  PI_Start
//
//  Created by Hélder Gonçalves on 06/03/15.
//  Copyright (c) 2015 Hélder Gonçalves. All rights reserved.
//

#include "lll.h"

double **baseORT;
double **coeffs;
double *norms;
double *projAux;

//Use (double **base, double **baseORT, double** coeffs, double* norms)
void initStructsLLL(double** base, int rows, int cols){
    int i;
    
    //Set size variables
    numVector = rows;
    dimVector = cols;
    
    //Allocate all necessary memory
    baseORT = (double**)_mm_malloc(numVector*sizeof(double*),64);
    coeffs = (double**)_mm_malloc(numVector*sizeof(double*),64);
    
    for(i=0; i<numVector; i++){
        
        //Base Orthogonal
        baseORT[i] = (double*)_mm_malloc(dimVector*sizeof(double),64);
        coeffs[i] = (double*)_mm_malloc(dimVector*sizeof(double),64);
    }
    
    norms = (double*)_mm_malloc(dimVector*sizeof(double),64);
    projAux = (double*)_mm_malloc(dimVector*sizeof(double),64);
    
}


void prepareBaseORT(double** base, int num){
    int i;
    
    for(i=0; i<num; i++)
        memccpy(baseORT[i], base[i], dimVector, sizeof(double));
}


void shiftVector(double** base, int k, int kl){
    
    int i;
    double *aux, *aux2;
    
    //insert B[kl] right before B[k]
    aux=base[k+1];
    base[k+1]=base[kl];
    
    
    //Correct the other pointers
    for (i=k+2; i<=kl; i++) {
        aux2 = base[k];
        base[k] = aux;
        aux = aux2;
    }
}

//Use (double *base[], double *baseORT[], double* coeffs[], double* norms)
void coefficientsGS(double *base[]){
    
    int i, j;
    
    for(i=numVector-1; i>0; i--){
        for (j=i-1; j>=0; j--) {
            coeffs[i][j]= innerProduct(&base[i][0], &baseORT[j][0], dimVector) / innerProduct(&baseORT[j][0], &baseORT[j][0], dimVector);
        }
    }
    
    for (i=0; i<dimVector; i++) {
        norms[i] = pow(vectorNorm(&baseORT[i][0], dimVector), 2);
    }
}


//Use (double* base[], double* baseORT[])
void gram_Schmidt_Orthonormalization(double* base[]){
    
    int i, k, j;
    
    for(k=0; k<numVector; k++){
        
        normalizeVector(baseORT[k], dimVector);
        
        for(i=k+1; k<numVector; k++){
            //remove component in direction Vk
            projection(baseORT[k], baseORT[i], projAux, dimVector);
            
            for(j=0; j<dimVector; j++)
                baseORT[k][j] -= projAux[j];
            
        }
    }
}

double breakCondition_Alg2(int k, int kl){
    
    int i;
    double res=norms[kl];
    
    for(i=k-1; i<= kl-1; i++ ){
        res += pow(coeffs[kl][i],2) * norms[i];
    }
    
    return res;
}



//Algorithm 1
void sizeReduction(int k){
    int i, j;
    
    for(i=k-1; i>=0; i--){
        
        for(j=0; j<dimVector; j++){
            baseORT[k][j] -= round(coeffs[k][i]) * baseORT[i][j];
        }
        
        for(j=0; j<i; j++){
            coeffs[k][j] -= round(coeffs[k][i])*coeffs[i][j];
        }
    }
}

//Algorithm 2 - Lenstra–Lenstra–Lovász
void lll(double* base[], double delta, int indiceMax){
    
    int i, k=1, kl;
    
    numVector = indiceMax;
    prepareBaseORT(base, numVector);
    
    //Compute orthogonal basis, Coefficients, norms
    gram_Schmidt_Orthonormalization(base);
    coefficientsGS(base);
    
    while(k<numVector){
        //sizeResuction and Update the Coeffs and norms
        sizeReduction(k);
        coefficientsGS(base);

        kl=k;
        while(k>=1 && (delta*norms[k-1] > breakCondition_Alg2(k, kl)) ){
            k--;
        }
        
        for(i=0; i<k-1; i++){
            coeffs[k][i] = coeffs[kl][i];
        }
        
        //Shift vectors
        shiftVector(base, k, kl);
        
        k++;
    }
}









