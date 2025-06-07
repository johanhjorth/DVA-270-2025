#include "grupp11.h"

//////////////////////////////////////
////////////// 11 ////////////////////
//////////////////////////////////////

char send_buffer[1024];
const static nrfx_uarte_t instance = NRFX_UARTE_INSTANCE(0);
const static nrfx_rtc_t rtc_instance = NRFX_RTC_INSTANCE(0);

static bool uarte_enabled = false;
static bool systick_enabled = false;
static bool rtc_enabled = false;

// Initializes URATE instace for communication
void init_uarte()
{
  NVIC_ClearPendingIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
  NVIC_EnableIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));

  nrfx_uarte_config_t config = NRFX_UARTE_DEFAULT_CONFIG(20, 22);
  config.baudrate = NRF_UARTE_BAUDRATE_1000000;

  // nrfx_err_t errr = nrfx_uarte_init(&instance, &config, NULL);
  nrfx_err_t errr = nrfx_uarte_init(&instance, &config, uarte_handler);
  assert(errr == NRFX_SUCCESS);

  uarte_enabled = true;
}

// Initializes SysTick for tick functions such as sleep
void init_systick()
{
  nrfx_systick_init();

  systick_enabled = true;
}

// Initializes RTC instace for Real Time Clock usage
void init_rtc()
{
  const nrfx_rtc_config_t config = NRFX_RTC_DEFAULT_CONFIG;

  nrfx_err_t errr = nrfx_rtc_init(&rtc_instance, &config, NULL);
  assert(errr == NRFX_SUCCESS);

  nrfx_rtc_enable(&rtc_instance);

  rtc_enabled = true;
}

/////////////////////////////////////////
/////////// SENDING FUNCTIONS ///////////
/////////////////////////////////////////

// Sends the given string of size over UARTE
void write_uarte(const uint8_t *data, const size_t length)
{
  assert(uarte_enabled);
  assert(data != NULL);

  if (length == 0)
  {
    return;
  }

  nrfx_uarte_tx(&instance, data, length, 0);
  while(nrfx_uarte_tx_in_progress(&instance));
}

// Sends the given string of size over UARTE
void send_text(char *string)
{
  assert(uarte_enabled);

  int length = 0;
  while (*(string + length) != 0) length++;

  assert(length <= sizeof(send_buffer));

  // Because it has to be done this way? Why, I don't know
  sprintf(send_buffer, "%s", string);

  write_uarte((const uint8_t*)send_buffer, length);
}

// Sends the given int as text over UARTE
void send_int(int value)
{
  assert(uarte_enabled);

  char string[11];
  sprintf(string, "%d", value);
  send_text(string);
}

/////////////////////////////////////////
/////////// READING FUNCTIONS ///////////
/////////////////////////////////////////

void read_uarte(uint8_t *data, const size_t length)
{
  assert(uarte_enabled);
  assert(data != NULL);

  if (length == 0)
  {
    return;
  }

  nrfx_uarte_rx(&instance, data, length);
}

uint32_t read_rtc_counter()
{
  assert(rtc_enabled);

  return nrfx_rtc_counter_get(&rtc_instance);
}

/////////////////////////////////////////
//////////// OTHER FUNCTIONS ////////////
/////////////////////////////////////////

// Blocking sleep for specified milliseconds
void sleep_ms(uint32_t milliseconds)
{
  assert(systick_enabled);

  nrfx_systick_delay_ms(milliseconds);
}

//////////////////////////////////////
////////// BoundingBox ///////////////
//////////////////////////////////////

BoundingBox globbo_bb = {
  .x = 2,
  .y = 4,
  .width = 12,
  .height = 9
};

BoundingBox pipe_top_up_bb = {
  .x = 3,
  .y = 7,
  .width = 10,
  .height = 9
};

BoundingBox pipe_body_bb = {
  .x = 3,
  .y = 0,
  .width = 10,
  .height = 16
};

BoundingBox pipe_top_down_bb = {
  .x = 3,
  .y = 0,
  .width = 10,
  .height = 9
};

bool does_collide(BoundingBox self, int self_x, int self_y, BoundingBox other, int other_x, int other_y)
{
  self.x += self_x;
  self.y += self_y;

  other.x += other_x;
  other.y += other_y;

  return self.x < other.x + other.width
    && self.x + self.width > other.x
    && self.y < other.y + other.height
    && self.y + self.height > other.y;
}

//////////////////////////////////////
//////////// SPRITES /////////////////
//////////////////////////////////////

//////////////////////
/////// GLOBBO ///////
//////////////////////

//Globbo
SPRITE_16x16(globbo)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0)
ROW_16(0,0,0,0,1,1,4,4,1,1,1,1,0,0,0,0)
ROW_16(0,0,0,1,1,4,1,1,1,1,2,1,1,0,0,0)
ROW_16(0,0,0,1,1,1,1,1,2,1,2,1,1,0,0,0)
ROW_16(0,0,1,1,1,1,1,1,2,1,1,1,1,1,0,0)
ROW_16(0,0,1,1,1,1,1,1,2,1,1,1,1,1,0,0)
ROW_16(0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0)
ROW_16(0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0)
ROW_16(0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
END_SPRITE()

//Globbo jump
SPRITE_16x16(globbo_jump)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0)
ROW_16(0,0,0,0,0,1,1,1,4,1,1,1,0,0,0,0)
ROW_16(0,0,0,0,1,1,4,1,1,1,2,1,1,0,0,0)
ROW_16(0,0,0,1,1,4,1,1,2,1,2,1,1,0,0,0)
ROW_16(0,0,0,1,1,1,1,1,2,1,1,1,1,1,0,0)
ROW_16(0,0,1,1,1,1,1,1,2,1,1,1,1,1,0,0)
ROW_16(0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0)
ROW_16(0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0)
ROW_16(0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0)
ROW_16(0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0)
ROW_16(0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
END_SPRITE()

//globbo fall
SPRITE_16x16(globbo_fall)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0)
ROW_16(0,1,0,0,1,1,1,1,4,1,1,1,0,0,0,0)
ROW_16(0,0,0,1,1,1,4,4,1,1,1,1,1,0,0,0)
ROW_16(0,0,1,1,1,1,1,1,1,1,2,1,1,1,0,0)
ROW_16(0,0,1,1,1,1,1,1,2,1,1,2,1,1,0,0)
ROW_16(0,0,1,1,1,1,1,1,2,1,1,1,1,1,0,0)
ROW_16(0,0,0,1,1,1,1,2,1,1,1,1,1,1,0,0)
ROW_16(0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0)
ROW_16(0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
END_SPRITE()

//////////////////////
//////// PIPE ////////
//////////////////////

//pipe top up
SPRITE_16x16(pipe_top_up)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,3,3,3,3,3,3,4,3,4,4,0,0,0)
ROW_16(0,0,0,3,3,3,3,3,3,3,3,3,3,0,0,0)
ROW_16(0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,0)
ROW_16(0,0,0,0,2,2,3,3,3,3,3,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,3,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,4,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,4,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,4,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,3,3,0,0,0,0)
END_SPRITE()

//pipe body
SPRITE_16x16(pipe_body)
ROW_16(0,0,0,0,2,3,3,3,3,3,3,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,4,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,4,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,4,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,3,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,3,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,4,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,3,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,4,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,4,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,4,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,3,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,3,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,4,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,4,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,3,3,0,0,0,0)
END_SPRITE()

//pipe top down
SPRITE_16x16(pipe_top_down)
ROW_16(0,0,0,0,2,3,3,3,3,3,3,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,4,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,4,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,4,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,3,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,3,3,3,3,0,0,0,0)
ROW_16(0,0,0,0,2,3,3,3,4,4,4,4,0,0,0,0)
ROW_16(0,0,0,2,3,3,3,3,3,3,3,3,4,0,0,0)
ROW_16(0,0,0,2,2,2,2,2,2,2,3,3,3,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
END_SPRITE()

///////////////////////
/////// NUMBERS ///////
///////////////////////

SPRITE_8x8(zero)
ROW_8(0,0,0,0,0,0,0,0)
ROW_8(0,0,0,4,4,0,0,0)
ROW_8(0,0,4,0,0,4,0,0)
ROW_8(0,0,4,4,0,4,0,0)
ROW_8(0,0,4,0,4,4,0,0)
ROW_8(0,0,4,0,0,4,0,0)
ROW_8(0,0,0,4,4,0,0,0)
ROW_8(0,0,0,0,0,0,0,0)
END_SPRITE()

SPRITE_8x8(one)
ROW_8(0,0,0,0,0,0,0,0)
ROW_8(0,0,0,0,4,0,0,0)
ROW_8(0,0,0,4,4,0,0,0)
ROW_8(0,0,0,0,4,0,0,0)
ROW_8(0,0,0,0,4,0,0,0)
ROW_8(0,0,0,0,4,0,0,0)
ROW_8(0,0,0,4,4,4,0,0)
ROW_8(0,0,0,0,0,0,0,0)
END_SPRITE()

SPRITE_8x8(two)
ROW_8(0,0,0,0,0,0,0,0)
ROW_8(0,0,4,4,4,0,0,0)
ROW_8(0,0,0,0,0,4,0,0)
ROW_8(0,0,0,0,0,4,0,0)
ROW_8(0,0,0,4,4,0,0,0)
ROW_8(0,0,4,0,0,0,0,0)
ROW_8(0,0,4,4,4,4,0,0)
ROW_8(0,0,0,0,0,0,0,0)
END_SPRITE()

SPRITE_8x8(three)
ROW_8(0,0,0,0,0,0,0,0)
ROW_8(0,0,4,4,4,0,0,0)
ROW_8(0,0,0,0,0,4,0,0)
ROW_8(0,0,0,4,4,0,0,0)
ROW_8(0,0,0,0,0,4,0,0)
ROW_8(0,0,0,0,0,4,0,0)
ROW_8(0,0,4,4,4,0,0,0)
ROW_8(0,0,0,0,0,0,0,0)
END_SPRITE()

SPRITE_8x8(four)
ROW_8(0,0,0,0,0,0,0,0)
ROW_8(0,0,4,0,0,0,0,0)
ROW_8(0,0,4,0,4,0,0,0)
ROW_8(0,0,4,4,4,4,0,0)
ROW_8(0,0,0,0,4,0,0,0)
ROW_8(0,0,0,0,4,0,0,0)
ROW_8(0,0,0,0,4,0,0,0)
ROW_8(0,0,0,0,0,0,0,0)
END_SPRITE()

SPRITE_8x8(five)
ROW_8(0,0,0,0,0,0,0,0)
ROW_8(0,0,4,4,4,0,0,0)
ROW_8(0,0,4,0,0,0,0,0)
ROW_8(0,0,4,4,4,0,0,0)
ROW_8(0,0,0,0,0,4,0,0)
ROW_8(0,0,0,0,0,4,0,0)
ROW_8(0,0,4,4,4,0,0,0)
ROW_8(0,0,0,0,0,0,0,0)
END_SPRITE()

SPRITE_8x8(six)
ROW_8(0,0,0,0,0,0,0,0)
ROW_8(0,0,0,4,4,0,0,0)
ROW_8(0,0,4,0,0,0,0,0)
ROW_8(0,0,4,4,4,0,0,0)
ROW_8(0,0,4,0,0,4,0,0)
ROW_8(0,0,4,0,0,4,0,0)
ROW_8(0,0,0,4,4,0,0,0)
ROW_8(0,0,0,0,0,0,0,0)
END_SPRITE()

SPRITE_8x8(seven)
ROW_8(0,0,0,0,0,0,0,0)
ROW_8(0,0,4,4,4,4,0,0)
ROW_8(0,0,0,0,0,4,0,0)
ROW_8(0,0,0,0,4,0,0,0)
ROW_8(0,0,0,4,0,0,0,0)
ROW_8(0,0,0,4,0,0,0,0)
ROW_8(0,0,0,4,0,0,0,0)
ROW_8(0,0,0,0,0,0,0,0)
END_SPRITE()

SPRITE_8x8(eight)
ROW_8(0,0,0,0,0,0,0,0)
ROW_8(0,0,0,4,4,0,0,0)
ROW_8(0,0,4,0,0,4,0,0)
ROW_8(0,0,0,4,4,0,0,0)
ROW_8(0,0,4,0,0,4,0,0)
ROW_8(0,0,4,0,0,4,0,0)
ROW_8(0,0,0,4,4,0,0,0)
ROW_8(0,0,0,0,0,0,0,0)
END_SPRITE()

SPRITE_8x8(nine)
ROW_8(0,0,0,0,0,0,0,0)
ROW_8(0,0,0,4,4,0,0,0)
ROW_8(0,0,4,0,0,4,0,0)
ROW_8(0,0,0,4,4,4,0,0)
ROW_8(0,0,0,0,0,4,0,0)
ROW_8(0,0,4,0,0,4,0,0)
ROW_8(0,0,0,4,4,0,0,0)
ROW_8(0,0,0,0,0,0,0,0)
END_SPRITE()

/////////////////////
/////// START ///////
/////////////////////
SPRITE_50(title_screen)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,0,0,0,0)
ROW_50(0,0,0,0,4,2,2,2,2,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,2,2,2,2,4,0,0,0,0)
ROW_50(0,0,0,0,4,2,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,2,4,0,0,0,0)
ROW_50(0,0,0,0,4,2,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,2,2,2,2,2,4,4,4,4,4,4,4,4,4,4,4,4,2,4,0,0,0,0)
ROW_50(0,0,0,0,4,2,4,4,4,2,4,4,4,4,4,4,4,4,4,4,2,2,2,4,4,4,4,4,4,2,4,4,4,4,4,2,2,2,4,2,2,2,4,4,2,4,0,0,0,0)
ROW_50(0,0,0,0,4,4,4,4,2,4,2,4,2,2,4,4,2,4,2,4,2,4,4,4,2,4,4,2,4,2,4,2,4,2,4,2,4,2,4,2,4,4,4,4,4,4,0,0,0,0)
ROW_50(0,0,0,0,4,4,4,4,2,2,2,4,2,4,2,4,2,4,2,4,2,2,4,4,2,2,4,2,4,2,4,2,4,2,4,2,2,2,4,2,2,4,4,4,4,4,0,0,0,0)
ROW_50(0,0,0,0,4,4,4,4,2,4,2,4,2,4,2,4,2,4,2,4,2,4,4,4,2,4,2,2,4,2,4,2,4,2,4,2,2,4,4,2,4,4,4,4,4,4,0,0,0,0)
ROW_50(0,0,0,0,4,4,4,4,2,4,2,4,2,2,4,4,4,2,4,4,2,2,2,4,2,4,4,2,4,2,4,2,2,2,4,2,4,2,4,2,2,2,4,4,4,4,0,0,0,0)
ROW_50(0,0,0,0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,0,0,0,0)
ROW_50(0,0,0,0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,2,2,2,4,2,2,2,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,0,0,0,0)
ROW_50(0,0,0,0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,2,4,2,4,2,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,0,0,0,0)
ROW_50(0,0,0,0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,2,4,2,4,2,2,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,0,0,0,0)
ROW_50(0,0,0,0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,2,2,2,4,2,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,0,0,0,0)
ROW_50(0,0,0,0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,0,0,0,0)
ROW_50(0,0,0,0,4,4,4,4,4,4,4,4,2,2,2,2,4,4,4,4,4,4,4,4,4,4,2,2,4,4,2,2,4,4,4,4,4,4,4,4,4,4,4,4,4,4,0,0,0,0)
ROW_50(0,0,0,0,4,4,4,4,4,4,4,4,2,4,4,4,4,2,4,4,4,4,2,2,2,4,2,4,2,4,2,4,2,4,2,2,2,4,4,4,4,4,4,4,4,4,0,0,0,0)
ROW_50(0,0,0,0,4,4,4,4,4,4,4,4,2,4,2,2,4,2,4,4,4,4,2,4,2,4,2,2,4,4,2,2,4,4,2,4,2,4,4,4,4,4,4,4,4,4,0,0,0,0)
ROW_50(0,0,0,0,4,4,4,4,4,4,4,4,2,4,4,2,4,2,4,4,4,4,2,4,2,4,2,4,2,4,2,4,2,4,2,4,2,4,4,4,4,4,4,4,4,4,0,0,0,0)
ROW_50(0,0,0,0,4,2,4,4,4,4,4,4,2,2,2,2,4,2,2,2,2,4,2,2,2,4,2,2,2,4,2,2,2,4,2,2,2,4,4,4,4,4,4,4,2,4,0,0,0,0)
ROW_50(0,0,0,0,4,2,4,4,4,4,4,4,4,4,4,2,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,2,4,0,0,0,0)
ROW_50(0,0,0,0,4,2,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,2,4,0,0,0,0)
ROW_50(0,0,0,0,4,2,2,2,2,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,2,2,2,2,4,0,0,0,0)
ROW_50(0,0,0,0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,0,0,0,0)
ROW_50(0,0,0,0,4,2,2,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,2,2,4,0,0,0,0)
ROW_50(0,0,0,0,4,2,4,4,4,4,4,4,4,4,2,2,2,4,2,2,2,2,2,4,4,4,4,4,4,4,4,2,2,2,2,2,4,4,4,4,4,4,4,4,2,4,0,0,0,0)
ROW_50(0,0,0,0,4,4,4,4,4,4,4,4,4,2,4,4,4,4,4,4,2,4,4,4,2,4,4,2,2,2,4,4,4,2,4,4,4,4,4,4,4,4,4,4,4,4,0,0,0,0)
ROW_50(0,0,0,0,4,4,4,4,4,4,4,4,4,4,2,2,4,4,4,4,2,4,4,2,4,2,4,2,4,2,4,4,4,2,4,4,4,4,4,4,4,4,4,4,4,4,0,0,0,0)
ROW_50(0,0,0,0,4,2,4,4,4,4,4,4,4,4,4,4,2,4,4,4,2,4,4,2,2,2,4,2,2,4,4,4,4,2,4,4,4,4,4,4,4,4,4,4,2,4,0,0,0,0)
ROW_50(0,0,0,0,4,2,2,4,4,4,4,4,4,2,2,2,4,4,4,4,2,4,4,2,4,2,4,2,4,2,4,4,4,2,4,4,4,4,4,4,4,4,4,2,2,4,0,0,0,0)
ROW_50(0,0,0,0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
END_SPRITE()

///////////////////////
////// GAME OVER //////
///////////////////////
SPRITE_50(game_over)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,0,0,0,0)
ROW_50(0,0,0,0,4,2,2,4,4,2,2,2,4,4,4,4,4,4,4,4,4,4,4,2,2,2,4,4,4,4,4,4,4,4,4,2,2,2,4,4,4,4,4,2,2,4,0,0,0,0)
ROW_50(0,0,0,0,4,2,4,4,2,4,4,4,4,4,2,4,4,2,4,4,4,2,4,2,4,4,4,2,2,2,4,2,4,2,4,2,4,4,4,2,2,2,4,4,2,4,0,0,0,0)
ROW_50(0,0,0,0,4,4,4,4,2,4,2,2,4,2,4,2,4,2,2,4,2,2,4,2,2,4,4,2,4,2,4,2,4,2,4,2,2,4,4,2,4,2,4,4,4,4,0,0,0,0)
ROW_50(0,0,0,0,4,4,4,4,2,4,4,2,4,2,2,2,4,2,4,2,4,2,4,2,4,4,4,2,4,2,4,2,4,2,4,2,4,4,4,2,2,4,4,4,4,4,0,0,0,0)
ROW_50(0,0,0,0,4,2,4,4,4,2,2,2,4,2,4,2,4,2,4,4,4,2,4,2,2,2,4,2,2,2,4,4,2,4,4,2,2,2,4,2,4,2,4,4,2,4,0,0,0,0)
ROW_50(0,0,0,0,4,2,2,4,4,4,4,2,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,2,2,4,0,0,0,0)
ROW_50(0,0,0,0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,4,4,4,4,4,4,2,2,2,4,4,4,4,4,4,4,4,4,4,4,4,4,2,2,2,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,4,4,4,4,4,2,4,4,4,4,2,2,2,4,2,2,2,4,2,2,2,4,2,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,4,4,4,4,4,4,2,2,4,4,2,4,4,4,2,4,2,4,2,4,2,4,2,2,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,4,4,4,4,4,4,4,4,2,4,2,4,4,4,2,4,2,4,2,2,4,4,2,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,4,4,4,4,4,2,2,2,4,4,2,2,2,4,2,2,2,4,2,4,2,4,2,2,2,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
ROW_50(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
END_SPRITE()

//////////////////////////////////////
///////////// SPRITE /////////////////
//////////////////////////////////////

char* ansi_colors[] = {
  CSI "30;40m",  // 0, Blank

  CSI "91;101m", // 1, Bright red
  CSI "90;100m", // 2, Bright black
  CSI "37;47m",  // 3, White
  CSI "97;107m", // 4, Bright white

  CSI "30;40m", // UNUSED
  CSI "30;40m", // UNUSED
  CSI "30;40m", // UNUSED
};

char draw_buffer[1024];
int draw_buffer_len = 0;

// Buffer the things to send to uarte so that there are fewer actual sends
void prep_send(const char* text, const size_t len)
{
  if (len + draw_buffer_len >= sizeof(draw_buffer))
  {
      draw_frame();
  }

  strcpy(draw_buffer + draw_buffer_len, text);
  draw_buffer_len += len;
}

// Flush the buffer
void draw_frame()
{
  write_uarte((uint8_t*)draw_buffer, draw_buffer_len);
  draw_buffer_len = 0;
}

// Moves the terminal cursor to a position on the screen
// Basically allows drawing at specific positions
// without needing to know the entire screen
void move_cursor(int x, int y)
{
  if (x < 1)
  {
    x = 1;
  }

  if (y < 1)
  {
    y = 1;
  }

  char buffer[32];
  sprintf(buffer, CSI "%d;%dH", y, x*2);
  prep_send(buffer, strlen(buffer));
}

// Hides or shows the terminal cursor
void show_cursor(bool show) {
  if (show)
  {
    send_text(CSI "?25h");
  }
  else
  {
    send_text(CSI "?25l");
  }
}

// Draws a pixel in the specified color
int current_color = 0;
void set_color(uint8_t color)
{
  // Check if the color/pixel to draw is blank
  if (color == 0)
  {
    current_color = color;
    prep_send(CSI "2C", sizeof(CSI "2C")); // If so, just skip and jump 2 pixels right
    return;
  }

  // Check if the pixel to draw is another color than the current one
  if (color != current_color)
  {
    switch (color)
    {
      case 1:
        prep_send(ansi_colors[1], 9);
        break;
      case 2:
        prep_send(ansi_colors[2], 9);
        break;
      case 3:
        prep_send(ansi_colors[3], 8);
        break;
      case 4:
        prep_send(ansi_colors[4], 9);
        break;
    }

    current_color = color;
  }

  // Actually draw the pixel
  prep_send("  ", 2);
}

// Renders an 8x8 sprite to the screen
// This is currently never off-screen
void render_sprite_8x8(const Sprite_8x8* sprite, int x, int y)
{
  // Move cursor to the start of the first line
  move_cursor(x, y);

  for (int y_off = 0; y_off < 8; y_off++)
  {
    for (int x_off = 0; x_off < 8; x_off += 2)
    {
      Pixels pixels = sprite->data[x_off/2 + y_off*4];

     set_color(pixels.left);
     set_color(pixels.right);
    }

    // Move to the next line
    move_cursor(x, y + y_off + 1);
  }
}

// Renders a 16x16 sprite to the screen
// May be partially/fully off-screen
void render_sprite_16x16(const Sprite_16x16* sprite, int x, int y)
{
  move_cursor(x, y);

  for (int y_off = 0; y_off < 16 && y + y_off <= TERMINAL_HEIGHT; y_off++)
  {
    // Off-screen on the top?
    if (y + y_off + 1 < 1)
    {
      continue;
    }

    for (int x_off = 0; x_off < 16; x_off += 2)
    {
      Pixels pixels = sprite->data[x_off/2 + y_off*8];

      // Off-screen?
      if (x + x_off < TERMINAL_WIDTH/2 && x + x_off > 0)
      {
       set_color(pixels.left);
      }

      // Off-screen?
      if (x + x_off + 1 < TERMINAL_WIDTH/2 && x + x_off + 1 > 0)
      {
       set_color(pixels.right);
      }
    }

    // Move to next line
    move_cursor(x, y + y_off + 1);
  }
}

// Renders a 50x50 sprite to the screen
// This is currently never off-screen
void render_sprite_50x50(const Sprite_50x50* sprite, int x, int y)
{
  move_cursor(x, y);

  for (int y_off = 0; y_off < 50; y_off++)
  {
    for (int x_off = 0; x_off < 50; x_off += 2)
    {
      Pixels pixels = sprite->data[x_off/2 + y_off*25];

     set_color(pixels.left);
     set_color(pixels.right);
    }

    // Move to next line
    move_cursor(x, y + y_off + 1);
  }
}

//////////////////////////////////////
////////////// PIPE //////////////////
//////////////////////////////////////

// Create a pipe at the edge of the screen
Pipe* generate_pipe()
{
  int y = RAND(MIN_PIPE_Y, MAX_PIPE_Y);
  int gap = RAND(MIN_PIPE_GAP_SIZE, MAX_PIPE_GAP_SIZE);

  Pipe* pipe = (Pipe*)malloc(sizeof(Pipe));
  pipe->x = TERMINAL_WIDTH/2;
  pipe->y = y;
  pipe->gap_size = gap;
  pipe->scored = false;

  return pipe;
}

// Draw the pipe to the screen
void draw_pipe(const Pipe* pipe)
{
  int y_off = (pipe->y%16) - 16;

  // Draw the top pipe's body
  for (int y = y_off; y < pipe->y - 16; y += 16)
  {
      render_sprite_16x16(&pipe_body, pipe->x, y);
  }

  // Draw the top pipe's tip
  render_sprite_16x16(&pipe_top_down, pipe->x, pipe->y - 16);
  // Draw the bottom pipe's tip
  render_sprite_16x16(&pipe_top_up, pipe->x, pipe->y + pipe->gap_size);

  // Draw the bottom pipe's body
  for (int y = pipe->y + pipe->gap_size + 16; y < TERMINAL_HEIGHT; y += 16)
  {
      render_sprite_16x16(&pipe_body, pipe->x, y);
  }
}


//////////////////////////////////////
////////////// GAME //////////////////
//////////////////////////////////////

Pipe* pipes[PIPES];

int score;
const Sprite_16x16* globbo_sprite = &globbo;
int y;
int y_speed;
uint8_t buffer;

// Handles keypresses
void uarte_handler(nrfx_uarte_event_t const * p_event, void * p_context)
{
  nrfx_uarte_t* p_inst = p_context;
  if (p_event->type == NRFX_UARTE_EVT_RX_DONE)
  {
    y_speed = -3;
  }
}

// Creates a new pipe in an available slot
void new_pipe()
{
  for (int i = 0; i < PIPES; i++)
  {
    if (pipes[i] == NULL)
    {
      pipes[i] = generate_pipe();
      break;
    }
  }
}

// Checks if a pipe collides with globbo
bool check_collision(const Pipe* pipe, const Sprite_16x16* globbo_sprite, const int globbo_x, const int globbo_y)
{
  int distance = (globbo_x - pipe->x) * (globbo_x - pipe->x);

  // Check if the distance between globbo and the pipe is too large
  // If so, don't bother checking
  if (distance > 256)
  {
    return false;
  }

  // Check the top pipe's body
  int y_off = (pipe->y%16) - 16;
  for (int y = y_off; y < pipe->y - 16; y += 16)
  {
    if (does_collide(globbo_bb, globbo_x, globbo_y, pipe_body_bb, pipe->x, y))
    {
      return true;
    }
  }

  // Check the top pipe's tip
  if (does_collide(globbo_bb, globbo_x, globbo_y, pipe_top_down_bb, pipe->x, pipe->y - 16))
  {
    return true;
  }

  // Check the bottom pipe's tip
  if (does_collide(globbo_bb, globbo_x, globbo_y, pipe_top_up_bb, pipe->x, pipe->y + pipe->gap_size))
  {
    return true;
  }

  // Check the bottom pipe's body
  for (int y = pipe->y + pipe->gap_size + 16; y < TERMINAL_HEIGHT; y += 16)
  {
    if (does_collide(globbo_bb, globbo_x, globbo_y, pipe_body_bb, pipe->x, y))
    {
      return true;
    }
  }

  return false;
}

void start_screen()
{
  send_text("\ec");
  show_cursor(false);

  render_sprite_50x50(&title_screen, 25, 25);
  draw_frame();

  // Await a keypress
  y_speed = 0;
  read_uarte(&buffer, 1);
  while(y_speed == 0);

  srand(read_rtc_counter());
}

// Renders a digit to the screen as a sprite and not just text
void render_digit(int digit, int x, int y)
{
  switch (digit)
  {
    case '0':
      render_sprite_8x8(&zero, x, y);
      break;
    case '1':
      render_sprite_8x8(&one, x, y);
      break;
    case '2':
      render_sprite_8x8(&two, x, y);
      break;
    case '3':
      render_sprite_8x8(&three, x, y);
      break;
    case '4':
      render_sprite_8x8(&four, x, y);
      break;
    case '5':
      render_sprite_8x8(&five, x, y);
      break;
    case '6':
      render_sprite_8x8(&six, x, y);
      break;
    case '7':
      render_sprite_8x8(&seven, x, y);
      break;
    case '8':
      render_sprite_8x8(&eight, x, y);
      break;
    case '9':
      render_sprite_8x8(&nine, x, y);
      break;
  }
}

void game_over_screen()
{
  render_sprite_50x50(&game_over, 25, 25);

  char buffer[12];
  sprintf(buffer, "%d", score);

  if (score > 99)
  {
    render_digit(buffer[0], 42, 60);
    render_digit(buffer[1], 47, 60);
    render_digit(buffer[2], 52, 60);
  }
  else if (score > 9)
  {
    render_digit('0', 42, 60);
    render_digit(buffer[0], 47, 60);
    render_digit(buffer[1], 52, 60);
  }
  else
  {
    render_digit('0', 42, 60);
    render_digit('0', 47, 60);
    render_digit(buffer[0], 52, 60);
  }

  // Don't forget to draw!
  draw_frame();

  sleep_ms(1000);

  // Await keypress
  y_speed = 0;
  read_uarte(&buffer, 1);
  while(y_speed == 0);
}

void run_game()
{
  // Set start conditions
  y_speed = 0;
  y = GLOBBO_START_Y;
  score = 0;

  // Clear pipes
  for (int i = 0; i < PIPES; i++)
  {
    Pipe* pipe = pipes[i];
    if (pipe == NULL)
    {
      continue;
    }
    free(pipe);
    pipes[i] = NULL;
  }

  // Create first pipe and start delay for next
  int pipe_delay = RAND(PIPE_MIN_DELAY, PIPE_MAX_DELAY);
  new_pipe();

  // Gameplay loop
  while (1)
  {
    read_uarte(&buffer, 1);

    // Time to create new pipe?
    if (pipe_delay-- <= 0)
    {
      pipe_delay = RAND(PIPE_MIN_DELAY, PIPE_MAX_DELAY);
      new_pipe();
    }

    // We haven't reached max speed yet
    if (y_speed < 5)
    {
      y_speed++;
    }

    // Clear screen and remove cursor (again)
    send_text("\ec\e[0m\r\n");
    show_cursor(false);

    send_int(score); // Write current score

    // Which globbo sprite should we use?
    if (y_speed == 0)
    {
      globbo_sprite = &globbo;
    }
    else if (y_speed > 0)
    {
      globbo_sprite = &globbo_fall;
    }
    else
    {
      globbo_sprite = &globbo_jump;
    }

    // Move and render globbo
    y += y_speed;
    if (y < -3)
    {
      // Reached the top
      y = -3;
      y_speed = 0;
    }

    render_sprite_16x16(globbo_sprite, GLOBBO_X, y);


    if (y + 28 >= TERMINAL_HEIGHT)
    {
      // Blobbo touched the ground
      return;
    }

    // Check collision with pipes
    for (int i = 0; i < PIPES; i++)
    {
      Pipe* pipe = pipes[i];
      if (pipe == NULL)
      {
        continue;
      }

      if (check_collision(pipe, globbo_sprite, GLOBBO_X, y))
      {
        return;
      }
    }

    // Draw and move pipes
    update_pipes();

    // Don't forget to render!
    draw_frame();

    // Sleep a bit to limit max FPS to 10 and so that the last pipe doesn't flicker too much
    sleep_ms(100);
  }
}

// Draw and move pipes
void update_pipes()
{
  for (int i = 0; i < PIPES; i++)
  {
    Pipe* pipe = pipes[i];
    if (pipe == NULL)
    {
      continue;
    }

    draw_pipe(pipe);
    pipe->x -= 2;

    // Is globbo past pipe?
    if (pipe->x < GLOBBO_X && pipe->scored == false)
    {
      pipe->scored = true;
      if (score < 999)
      {
        score++;
      }
    }

    // Check if pipe is fully off-screen
    if (pipe->x <= -16)
    {
      free(pipe);
      pipes[i] = NULL;
    }
  }
}


//////////////////////////////////////
////////////// MAIN //////////////////
//////////////////////////////////////

void start_game_grupp11(void)
{
  init_systick();
  init_rtc();
  init_uarte();

  start_screen();

  while (1)
  {
    run_game();
    game_over_screen();
  }
}