#include <furi.h>
#include <furi_hal.h>
#include "notification.h"
#include "notification_messages.h"

#define NOTIFICATION_LED_COUNT 3
#define NOTIFICATION_EVENT_COMPLETE 0x00000001U

typedef enum {
    NotificationLayerMessage,
    InternalLayerMessage,
    SaveSettingsMessage,
} NotificationAppMessageType;

typedef struct {
    const NotificationSequence* sequence;
    NotificationAppMessageType type;
    FuriEventFlag* back_event;
} NotificationAppMessage;

typedef enum {
    LayerInternal = 0,
    LayerNotification = 1,
    LayerMAX = 2,
} NotificationLedLayerIndex;

typedef struct {
    uint8_t value_last[LayerMAX];
    uint8_t value[LayerMAX];
    NotificationLedLayerIndex index;
    Light light;
} NotificationLedLayer;

#define NOTIFICATION_SETTINGS_VERSION 0x01
#define NOTIFICATION_SETTINGS_MAGIC 0x16
#define NOTIFICATION_SETTINGS_OLD_PATH INT_PATH(".notification.settings")
#define NOTIFICATION_SETTINGS_PATH CFG_PATH("notification.settings")

typedef struct {
    uint8_t version;
    float display_brightness;
    float led_brightness;
    float speaker_volume;
    uint32_t display_off_delay_ms;
    int8_t contrast;
    bool vibro_on;
} NotificationSettings;

struct NotificationApp {
    FuriMessageQueue* queue;
    FuriPubSub* event_record;
    FuriTimer* display_timer;

    NotificationLedLayer display;
    NotificationLedLayer led[NOTIFICATION_LED_COUNT];
    uint8_t display_led_lock;

    NotificationSettings settings;
};

#ifdef __cplusplus
extern "C" {
#endif

void notification_message_save_settings(NotificationApp* app);

#ifdef __cplusplus
}
#endif