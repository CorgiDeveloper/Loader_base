#include "Misc.h"
#include "Security.h"

int main()
{
	// Security
	CheckAdmin();
	CheckMain();

	// Setup
	CursorFlag(false);
	RandomTitle();

	// Launched notification | Do the webhook yourself
	Webhook_Launched();
}