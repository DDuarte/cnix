#ifndef __SPEAKER_H
#define __SPEAKER_H

/** @defgroup speaker speaker
 * @{
 *
 * Functions that control the speaker output
 */

/**
 * @brief Enables/disables speaker
 *
 * Enables/disables speaker, by both enabling/disabling Timer 2 and its output gate
 *
 * @param on If zero disable speaker, otherwise enable it
 * @return Return 0 upon success and non-zero otherwise
 */
int speaker_ctrl(unsigned char on);

/**
 * @brief Programs speaker to generate tone for the given time interval
 *
 * @param freq Frequency of tone to generate in Hz
 * @param time Duration of tone in s
 * @return Return 0 upon success and non-zero otherwise
 */
int speaker_test(unsigned long freq, unsigned long time);

/**@}*/

#endif /* __SPEAKER_H */
