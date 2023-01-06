# BattleNetCookieLogin
Connect with a cookie on battle.net launcher

# What it does
 + Ask you for a cookie, encrypt it like battle.net does, then overwrite all values in "HKEY_CURRENT_USER\SOFTWARE\Blizzard Entertainment\Battle.net\UnifiedAuth" (these values are only useful for auto-login)

# Any issues?
 + Make sure that you are logged in with "Keep logged in" option on battle.net.launcher, otherwise the program won't be able to find the value to overwrite, since battle.net remove it when you are disconnected
 + If it still doesn't work, open an issue
 
# Preview video
https://user-images.githubusercontent.com/47749428/210692000-ba670217-c99c-464d-88b7-c5c76c0d5114.mp4
