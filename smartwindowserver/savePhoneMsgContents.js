// contents of the savePhoneMsgContents function block
context.global.summerMode = msg.payload.summerMode;
context.global.winterMode = msg.payload.winterMode;
context.global.manualVal = msg.payload.manualVal;
context.global.manualMode = msg.payload.manualMode;
context.global.automaticMode = msg.payload.automaticMode;
return msg;
