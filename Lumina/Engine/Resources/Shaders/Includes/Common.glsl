
//// ---------------------------------------------------------------------------------
////  Constants

const float PI = 3.14159265359;
const float TWO_PI = 6.28318530718;
const float INV_PI = 0.31830988618;


// Check if *any* of the bits in 'flag' are set in 'value'
bool HasFlag(uint value, uint flag)
{
    return (value & flag) != 0u;
}

// Check if *all* bits in 'flag' are set in 'value'
bool HasAllFlags(uint value, uint flag)
{
    return (value & flag) == flag;
}

// Set bits in 'flag' on 'value'
uint SetFlag(uint value, uint flag)
{
    return value | flag;
}

// Clear bits in 'flag' from 'value'
uint ClearFlag(uint value, uint flag)
{
    return value & (~flag);
}

// Toggle bits in 'flag' on 'value'
uint ToggleFlag(uint value, uint flag)
{
    return value ^ flag;
}