#include "stm32flash.h"

bool Stm32Flash::setReadOutProtectionEnabled(bool enabled)
{
    if (waitForPreviousOperation() != WaitOk)
        return false;

    // Authorizes the small information block programming
    FLASH.OPTKEYR = KEY1;
    FLASH.OPTKEYR = KEY2;

    FLASH.CR |= (1 << 5);   // OPTER: Option byte erase
    FLASH.CR |= (1 << 6);   // STRT: start the erase operation
    if (waitForPreviousOperation() != WaitOk)
        return false;
    FLASH.CR &= ~(1 << 5);  // OPTER: Option byte erase

    FLASH.CR |= (1 << 4);   // OPTPG: Option byte programming
    FLASH_OB.RDP = enabled ? 0x00 : READOUT_PROTECT_KEY;
    waitForPreviousOperation();
    FLASH.CR &= ~(1 << 4);  // OPTPG: Option byte programming

    return true;
}

/*
 * Flash hardware programs one half word at a time.
 * beginProgramming() must have been called prior.
 */
bool Stm32Flash::programHalfWord(uint16_t halfword, uint32_t address)
{
    if (waitForPreviousOperation() != WaitOk)
        return false;

    volatile uint16_t *pData = reinterpret_cast<uint16_t*>(address);
    *pData = halfword;

    return true;
}

Stm32Flash::WaitStatus Stm32Flash::waitForPreviousOperation()
{
    // wait for busy, but break on error
    while ((FLASH.SR & 0x1f) == 0x1)
        ;

    return (FLASH.SR & 0x1f) ? WaitError : WaitOk;
}

bool Stm32Flash::erasePage(uint32_t address)
{
    if (waitForPreviousOperation() != WaitOk)
        return false;

    FLASH.AR = address;
    FLASH.CR |= (1 << 6);   // STRT: start the erase operation

    return true;
}
