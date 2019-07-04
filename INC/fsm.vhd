-- fsm.vhd: Finite State Machine
-- Author(s): Klara Necasova (xnecas24)
--
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (STATE1, STATE2, STATE3, STATE4, STATE5, STATE6, STATE7, STATE8, STATE9, STATE10, STATE11, STATE12, STATE13, STATE14, FINAL_STATE, PRINT_ERR_MSG, PRINT_OK_MSG, FINISH, FAIL);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= STATE1;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when STATE1 =>
      next_state <= STATE1;
      if (KEY(1) = '1') then
         next_state <= STATE2; 
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR_MSG;
			elsif (KEY(15 downto 0) /= "0000000000000000") then
				 next_state <= FAIL; 
      end if;   
   -- - - - - - - - - - - - - - - - - - - - - - -
    when STATE2 =>
      next_state <= STATE2;
      if (KEY(6) = '1') then
         next_state <= STATE3; 
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR_MSG;
			elsif (KEY(15 downto 0) /= "0000000000000000") then
				 next_state <= FAIL; 
      end if; 
   -- - - - - - - - - - - - - - - - - - - - - - -
    when STATE3 =>
      next_state <= STATE3;
      if (KEY(5) = '1') then
         next_state <= STATE4; 
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR_MSG;
			elsif (KEY(15 downto 0) /= "0000000000000000") then
				 next_state <= FAIL; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
    when STATE4 =>
      next_state <= STATE4;
      if (KEY(8) = '1') then
         next_state <= STATE5; 
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR_MSG;
			elsif (KEY(15 downto 0) /= "0000000000000000") then
				 next_state <= FAIL; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
    when STATE5 =>
      next_state <= STATE5;
      if (KEY(3) = '1') then
         next_state <= STATE6; 
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR_MSG;
			elsif (KEY(15 downto 0) /= "0000000000000000") then
				 next_state <= FAIL; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
    when STATE6 =>
      next_state <= STATE6;
      if (KEY(2) = '1') then
         next_state <= STATE7; 
      elsif (KEY(6) = '1') then
         next_state <= STATE11;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR_MSG;
			elsif (KEY(15 downto 0) /= "0000000000000000") then
				 next_state <= FAIL; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
    when STATE7 =>
      next_state <= STATE7;
      if (KEY(0) = '1') then
         next_state <= STATE8; 
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR_MSG;
			elsif (KEY(15 downto 0) /= "0000000000000000") then
				 next_state <= FAIL; 
      end if; 
   -- - - - - - - - - - - - - - - - - - - - - - -
    when STATE8 =>
      next_state <= STATE8;
      if (KEY(0) = '1') then
         next_state <= STATE9; 
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR_MSG;
			elsif (KEY(15 downto 0) /= "0000000000000000") then
				 next_state <= FAIL; 
      end if;	
   -- - - - - - - - - - - - - - - - - - - - - - -
    when STATE9 =>
      next_state <= STATE9;
      if (KEY(9) = '1') then
         next_state <= STATE10; 
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR_MSG;
			elsif (KEY(15 downto 0) /= "0000000000000000") then
				 next_state <= FAIL; 
      end if;	
   -- - - - - - - - - - - - - - - - - - - - - - -
    when STATE10 =>
      next_state <= STATE10;
      if (KEY(6) = '1') then
         next_state <= FINAL_STATE; 
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR_MSG;
			elsif (KEY(15 downto 0) /= "0000000000000000") then
				 next_state <= FAIL; 
      end if;	
   -- - - - - - - - - - - - - - - - - - - - - - -
    when STATE11 =>
      next_state <= STATE11;
      if (KEY(0) = '1') then
         next_state <= STATE12; 
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR_MSG;
			elsif (KEY(15 downto 0) /= "0000000000000000") then
				 next_state <= FAIL; 
      end if;  
   -- - - - - - - - - - - - - - - - - - - - - - -
    when STATE12 =>
      next_state <= STATE12;
      if (KEY(2) = '1') then
         next_state <= STATE13; 
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR_MSG;
			elsif (KEY(15 downto 0) /= "0000000000000000") then
				 next_state <= FAIL; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
    when STATE13 =>
      next_state <= STATE13;
      if (KEY(8) = '1') then
         next_state <= STATE14; 
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR_MSG;
			elsif (KEY(15 downto 0) /= "0000000000000000") then
				 next_state <= FAIL; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
    when STATE14 =>
      next_state <= STATE14;
      if (KEY(8) = '1') then
         next_state <= FINAL_STATE; 
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR_MSG;
			elsif (KEY(15 downto 0) /= "0000000000000000") then
				 next_state <= FAIL; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
    when FINAL_STATE =>
      next_state <= FINAL_STATE;
      if (KEY(15) = '1') then
         next_state <= PRINT_OK_MSG; 
			elsif (KEY(15 downto 0) /= "0000000000000000") then
				 next_state <= FAIL; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
    when FAIL =>
      next_state <= FAIL;
      if (KEY(15) = '1') then
         next_state <= PRINT_ERR_MSG; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
    when FINISH =>
      next_state <= FINISH;
      if (KEY(15) = '1') then
         next_state <= STATE1; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
    when PRINT_OK_MSG =>
      next_state <= PRINT_OK_MSG;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
    when PRINT_ERR_MSG =>
      next_state <= PRINT_ERR_MSG;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -  
   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';
   FSM_MX_MEM     <= '0';
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';

   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FAIL =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_ERR_MSG =>
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_OK_MSG =>
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
      FSM_MX_MEM     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
     if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   end case;
end process output_logic;

end architecture behavioral;

