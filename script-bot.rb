require 'rubygems'
require 'watir'
require 'rails'

class ScriptBot
  username = 'type-your-email-here'
  password = 'type-your-pass-here'

  r = 11
  while true do
    if r > 105
      r = 11
    end
    puts "Current player: #{r}"

    client = Selenium::WebDriver::Remote::Http::Default.new
    client.read_timeout = 180 # seconds – default is 60
    begin
      browser = Watir::Browser.new :chrome, :http_client => client
      browser.goto 'http://booking.riddles.io/competitions/hack-man'

      Watir::Wait.while { browser.link(class: "Link--logIn").present? }
      browser.link(class: "Link--logIn").click

      browser.text_field(id: "email").set username
      browser.text_field(id: "password").set password
      browser.form(class: 'js-loginForm').submit

      sleep 4

      browser.goto 'http://booking.riddles.io/competitions/hack-man/leaderboard'
      Watir::Wait.while { browser.div(class: "Leaderboard-rank").present? }

      browser.div({class: "Leaderboard-rank", text: "#{r}"}).click
      person = browser.div(class: "is-expanded")
      
      person.button(class: "Button Button--outline Leaderboard-challengeButton").click

      sleep 4
      browser.close
    rescue Timeout::Error, Watir::Wait::TimeoutError, Watir::Exception::UnknownObjectException, Watir::Exception::ObjectDisabledException
      r += 1
      browser.close
      next
    end

    r += 1
    sleep 15
  end
end