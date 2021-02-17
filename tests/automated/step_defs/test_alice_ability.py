# coding=utf-8
"""Air Monitor feature tests."""

from pytest_bdd import (
    given,
    scenario,
    then,
    when,
)


@scenario('features/Alice-ability.feature', 'Alice controls Air monitor light')
def test_alice_controls_air_monitor_light():
    """Alice controls Air monitor light."""


@scenario('features/Alice-ability.feature', 'Alice says air quality')
def test_alice_says_air_quality():
    """Alice says air quality."""


@given('Alice ability is activated')
def alice_ability_is_activated():
    """Alice ability is activated."""
    raise NotImplementedError


@given('Metrics are stored')
def metrics_are_storedand_alert_are_configured_for_metrics():
    """Metrics are stored"""
    raise NotImplementedError


@given('Metrics are stored')
def metrics_are_stored():
    """Metrics are stored"""
    raise NotImplementedError

@when('an User say the light control commands')
def an_user_say_the_light_control_commands():
    """an User say the light control commands."""
    raise NotImplementedError


@when('the Alice ability is activated')
def the_alice_ability_is_activated():
    """the Alice ability is activated."""
    raise NotImplementedError


@then('Alice greetings with air quality report')
def alice_greetings_with_air_quality_report():
    """Alice greetings with air quality report."""
    raise NotImplementedError


@then('An User is notified')
def an_user_is_notified():
    """An User is notified."""
    raise NotImplementedError


@then('The Metrics are displayed')
def the_metrics_are_displayed():
    """The Metrics are displayed."""
    raise NotImplementedError


@then('The Metrics are stored')
def the_metrics_are_stored():
    """The Metrics are stored."""
    raise NotImplementedError


@then('the Air monitor device execute commands')
def the_air_monitor_device_execute_commands():
    """the Air monitor device execute commands."""
    raise NotImplementedError


@given('Yandex Monitoring has got the Air metrics')
def yandex_monitoring_has_got_the_air_metrics():
    """Yandex Monitoring has got the Air metrics."""
    raise NotImplementedError