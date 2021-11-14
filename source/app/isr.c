/*_____________________________________________________________________________
 │                                                                            |
 │ COPYRIGHT (C) 2021 Mihai Baneu                                             |
 │                                                                            |
 | Permission is hereby  granted,  free of charge,  to any person obtaining a |
 | copy of this software and associated documentation files (the "Software"), |
 | to deal in the Software without restriction,  including without limitation |
 | the rights to  use, copy, modify, merge, publish, distribute,  sublicense, |
 | and/or sell copies  of  the Software, and to permit  persons to  whom  the |
 | Software is furnished to do so, subject to the following conditions:       |
 |                                                                            |
 | The above  copyright notice  and this permission notice  shall be included |
 | in all copies or substantial portions of the Software.                     |
 |                                                                            |
 | THE SOFTWARE IS PROVIDED  "AS IS",  WITHOUT WARRANTY OF ANY KIND,  EXPRESS |
 | OR   IMPLIED,   INCLUDING   BUT   NOT   LIMITED   TO   THE  WARRANTIES  OF |
 | MERCHANTABILITY,  FITNESS FOR  A  PARTICULAR  PURPOSE AND NONINFRINGEMENT. |
 | IN NO  EVENT SHALL  THE AUTHORS  OR  COPYRIGHT  HOLDERS  BE LIABLE FOR ANY |
 | CLAIM, DAMAGES OR OTHER LIABILITY,  WHETHER IN AN ACTION OF CONTRACT, TORT |
 | OR OTHERWISE, ARISING FROM,  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR  |
 | THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                 |
 |____________________________________________________________________________|
 |                                                                            |
 |  Author: Mihai Baneu                           Last modified: 15.May.2020  |
 |                                                                            |
 |___________________________________________________________________________*/

#include "stm32f4xx.h"
#include "isr.h"

void isr_init()
{
    /* mask the EXTI lines as interupts */
    MODIFY_REG(EXTI->IMR, EXTI_IMR_MR0_Msk, EXTI_IMR_MR0);
    MODIFY_REG(EXTI->IMR, EXTI_IMR_MR1_Msk, EXTI_IMR_MR1);
    MODIFY_REG(EXTI->IMR, EXTI_IMR_MR2_Msk, EXTI_IMR_MR2);

    /* set rising edge as trigger */
    MODIFY_REG(EXTI->RTSR, EXTI_RTSR_TR0_Msk, EXTI_RTSR_TR0);
    MODIFY_REG(EXTI->FTSR, EXTI_FTSR_TR0_Msk, EXTI_FTSR_TR0);
    MODIFY_REG(EXTI->RTSR, EXTI_RTSR_TR1_Msk, EXTI_RTSR_TR1);
    MODIFY_REG(EXTI->FTSR, EXTI_FTSR_TR1_Msk, EXTI_FTSR_TR1);
    MODIFY_REG(EXTI->RTSR, EXTI_RTSR_TR2_Msk, EXTI_RTSR_TR2);
    MODIFY_REG(EXTI->FTSR, EXTI_FTSR_TR2_Msk, EXTI_FTSR_TR2);

    /* enable interupt */
    NVIC_SetPriority(EXTI0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 11 /* PreemptPriority */, 0 /* SubPriority */));
    NVIC_SetPriority(EXTI1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 11 /* PreemptPriority */, 0 /* SubPriority */));
    NVIC_SetPriority(EXTI2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 11 /* PreemptPriority */, 0 /* SubPriority */));

    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);
    NVIC_EnableIRQ(EXTI2_IRQn);
}

void EXTI0_IRQHandler(void)
{
  gpio_handle_trigger();
  SET_BIT(EXTI->PR, EXTI_PR_PR0_Msk);
}

void EXTI1_IRQHandler(void)
{
  gpio_handle_trigger();
  SET_BIT(EXTI->PR, EXTI_PR_PR1_Msk);
}

void EXTI2_IRQHandler(void)
{
  //gpio_handle_trigger();
  SET_BIT(EXTI->PR, EXTI_PR_PR2_Msk);
}
