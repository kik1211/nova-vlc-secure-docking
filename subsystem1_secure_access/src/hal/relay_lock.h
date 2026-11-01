/**
 * @file    relay_lock.h
 * @brief   Relay/solenoid lock HAL — initialization and timed unlock control.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * @version 1.0.0
 *
 * Controls the 5V relay module that switches the 12V solenoid lock circuit.
 * Provides non-blocking timed unlock via millis() comparison.
 */

#ifndef NOVA_RELAY_LOCK_H
#define NOVA_RELAY_LOCK_H

#include <stdbool.h>

/* === Public API === */

/**
 * @brief   Initialize relay GPIO pin and ensure door is locked.
 */
void relay_init(void);

/**
 * @brief   Assert relay (unlock door). Non-blocking — call relay_tick()
 *          to manage auto-relock timing.
 */
void relay_unlock(void);

/**
 * @brief   Deassert relay (lock door) immediately.
 */
void relay_lock(void);

/**
 * @brief   Non-blocking tick — call from main loop.
 *          Automatically relocks after DOOR_OPEN_DURATION_MS.
 * @return  true if the door is currently unlocked.
 */
bool relay_tick(void);

/**
 * @brief   Query whether the door is currently unlocked.
 * @return  true if relay is asserted (door open).
 */
bool relay_isUnlocked(void);

#endif /* NOVA_RELAY_LOCK_H */
