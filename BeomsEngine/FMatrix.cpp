#include "pch.h"
#include "FMatrix.h"

const FMatrix FMatrix::Identity(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
);

const FMatrix FMatrix::Zero(
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f
);